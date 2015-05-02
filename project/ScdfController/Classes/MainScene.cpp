
#include "SCDFCDefinitions.h"
#include "SCDFCScrollView.h"
#include "SCDFCItems.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "MultiSender.h"

using namespace ScdfCtrl;
USING_NS_CC;
using namespace ui;

int MainScene::gridIndex=0;
std::vector<float> MainScene::gridUnity;

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MainScene::create();

    scene->addChild(layer);
    return scene;
}

void MainScene::SnapToGrid(cocos2d::Rect &r)
{
    int newx=r.origin.x/GetGridDistance();
    if (((int)r.origin.x%GetGridDistance())>GetGridDistance()/2)
        newx+=1;
    int newy=r.origin.y/GetGridDistance();
    if (((int)r.origin.y%GetGridDistance())>GetGridDistance()/2)
        newy+=1;
    r.origin.x=newx*GetGridDistance();
    r.origin.y=newy*GetGridDistance();
}

template <class ItemType> void MainScene::OnStartDragging(cocos2d::Vec2 dragStartPoint)
{
}

template <class ItemType> void MainScene::OnDragging(cocos2d::Rect r)
{
    if (NULL==customPanel.get()) return;
    
    float scaledHeight=ItemType::GetBaseSize().height*GetGridDistance()+ITEMS_LABEL_HEIGHT; //item label
    if (ItemType::ID()!=ITEM_KNOB_ID && ItemType::ID()!=ITEM_KEYBOARD_ID && ItemType::ID()!=ITEM_SWITCH_ID)
        scaledHeight+=ITEMS_LABEL_HEIGHT;   //item control icon on top
    float scaledWidth=ItemType::GetBaseSize().width*GetGridDistance();

    r.origin=customPanel->convertToNodeSpace(r.origin);
    SnapToGrid(r);
    r.size=cocos2d::Size(scaledWidth, scaledHeight);

    customPanel->DetectCollisions(r);
    customPanel->SetDraggingRect(r);
}

template <class ItemType> void MainScene::OnEndDragging()
{
    if (NULL==customPanel.get()) return;
    customPanel->CheckAddControl<ItemType>();
    customPanel->SetDraggingRect(cocos2d::Rect::ZERO);
}

void MainScene::AttachItem(ItemBase *item)
{
    item->Attach(propertiesPanel.get());
    item->Notify();
}

void MainScene::DetachItem(ItemBase *item)
{
    item->Detach(propertiesPanel.get());
    propertiesPanel->Update(NULL, SCDFC_EVENTS_Remove_Item);
}

void MainScene::EnableScrollView(bool enable)
{
    if (enable)
        customScrollView->SetDirection(ScrollView::Direction::VERTICAL);
    else
        customScrollView->SetDirection(ScrollView::Direction::NONE);
}

void MainScene::CalculateGrid(cocos2d::Size workingPanelSize)
{
    gridUnity.clear();
    float divisore=10;
    float width=workingPanelSize.width;
    float height=workingPanelSize.height;
    for (;divisore<1000;){
        if ((0==std::fmod(width,divisore)) && (0==std::fmod(height,divisore)))
            gridUnity.push_back(divisore);
        divisore=divisore+0.5f;
    }
    if (0==gridUnity.size())
        gridUnity.push_back(GetUnityBase());
}

float MainScene::GetUnityBase()
{
    return 16.0;
}

void MainScene::UpdateMIDIDevicesMenu()
{
    if (propertiesPanel.get())
        propertiesPanel->UpdateDevicesMenu();
}

int MainScene::GetGridDistance()
{
    if (0==gridUnity.size()){
        printf("Error creating grid\n");
        exit(EXIT_FAILURE);
    }
    return gridUnity[gridIndex];
}

bool MainScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Scdf::MidiOutConnection::AttachMidiDeviceMenuListener(this);
    Director::getInstance()->setDisplayStats(false);

#define SCROLLVIEW_WIDTH (6.0*GetUnityBase())
#define PROPERTIES_WIDTH (16.0*GetUnityBase())

    cocos2d::Rect workingPanelsize(-(getContentSize().width/2.0),
                          3.0*getContentSize().height/2.0,
                          2*getContentSize().width,
                          2*getContentSize().height);
    cocos2d::Rect scrollViewect(getContentSize().width-(SCROLLVIEW_WIDTH*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE),
                       getContentSize().height/*-toolbarPanelsize.size.height*/,
                       SCROLLVIEW_WIDTH,
                       getContentSize().height/*-toolbarPanelsize.size.height*/);
    cocos2d::Rect propertiesRect(PROPERTIES_WIDTH*(PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE-1.0),
                       getContentSize().height/*-toolbarPanelsize.size.height*/,
                       PROPERTIES_WIDTH,
                       getContentSize().height/*-toolbarPanelsize.size.height*/);

    customPanel.reset(WorkingPanel::CreateCustomPanel((MainScene*)this,workingPanelsize));
    
    LOGD("Created custom panel");
    
    customScrollView.reset(ItemScrollView::CreateCustomScrollView((MainScene*)this, scrollViewect));
    
    LOGD("Created custom scrollview");

    propertiesPanel.reset(dynamic_cast<PropertiesPanel*>(PanelBase::CreatePanel<PropertiesPanel>((MainScene*)this,propertiesRect)));

    LOGD("Added toolbar");

    CalculateGrid(customPanel->getContentSize());

    LOGD("Calculated grid");
    
    HideShowPropertiesPanel(false);
    customScrollView->HideShow(false);
    
    return true;
}

void MainScene::HideShowPropertiesPanel(bool hide)
{
    //customPanel->SetActive(hide);
    propertiesPanel->HideShow(hide);
    propertiesPanel->Update(NULL, SCDFC_EVENTS_Update);
}

//void MainScene::OnGridButtonClick()
//{
//    gridIndex=++gridIndex%gridUnity.size();
//}

void MainScene::UpdateEditButton()
{
    propertiesPanel->UpdateEditButton(!customPanel->IsActive());
}

void MainScene::touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    static Vec2 dragStartUpdated;
    static bool useTouchDownToOpenClosePanel=true;
    Node* node = dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            switch(node->getTag())
            {
                case MAIN_BUTTON_NEW:
                {
                    SaveAfterNewPanel *p=SaveAfterNewPanel::create();
                    p->SetCallback(customPanel.get());
                    addChild(p,100);
                }
                    break;
                case MAIN_BUTTON_SAVE:
                {
                    SavePanel *p=SavePanel::create();
                    p->SetCallback(customPanel.get());
                    addChild(p,100);
                }
                    break;
                case MAIN_BUTTON_LOAD:
                {
                    LoadPanel *p=LoadPanel::create();
                    p->SetCallback(customPanel.get());
                    addChild(p,100);
                }
                    break;
                case MAIN_BUTTON_EDIT:
                    customPanel->SetActive(!customPanel->IsActive());
                    break;
                case MAIN_BUTTON_HIDESHOW_SCROLLVIEW:
                case MAIN_BUTTON_HIDESHOW_PROPERTIES:
                    dragStartUpdated=dynamic_cast<Widget*>(node)->getTouchBeganPosition();
                    useTouchDownToOpenClosePanel=true;
                    break;
                default:
                    break;
            }
            break;
        case Widget::TouchEventType::MOVED:
        {
            if (node->getTag()!=MAIN_BUTTON_HIDESHOW_SCROLLVIEW &&
                node->getTag()!=MAIN_BUTTON_HIDESHOW_PROPERTIES)
                break;

            useTouchDownToOpenClosePanel=false;
            
            Widget *w=dynamic_cast<Widget*>(node);
            float diff_x=dragStartUpdated.x-w->getTouchMovePosition().x;
            
            float newPosX=0;
            Node *panel=NULL;
            
            switch(node->getTag())
            {
                case MAIN_BUTTON_HIDESHOW_SCROLLVIEW:
                    panel=customScrollView.get();
                    newPosX=fmin(getContentSize().width-(panel->getContentSize().width*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE), fmax(getContentSize().width-panel->getContentSize().width, panel->getPositionX()-diff_x));
                    break;
                case MAIN_BUTTON_HIDESHOW_PROPERTIES:
                    panel=propertiesPanel.get();
                    newPosX=fmax(-panel->getContentSize().width+(panel->getContentSize().width*PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE), fmin(0, panel->getPositionX()-diff_x));
                    break;
                default:
                    break;
            }
            panel->setPosition(Vec2(newPosX, panel->getPositionY()));
            dragStartUpdated=w->getTouchMovePosition();
        }
            break;
        case Widget::TouchEventType::CANCELED:
        case Widget::TouchEventType::ENDED:
            switch(node->getTag())
            {
                case MAIN_BUTTON_HIDESHOW_SCROLLVIEW:
                {
                    bool hidePanel;
                    if (useTouchDownToOpenClosePanel)
                        hidePanel=customScrollView->IsOpened();
                    else{
                        if (customScrollView->IsOpened())
                            hidePanel=customScrollView->getPositionX()>=(getContentSize().width-3*customScrollView->getContentSize().width/4.0);
                        else
                            hidePanel=(customScrollView->getPositionX()+(customScrollView->getContentSize().width*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE))>=(getContentSize().width-customScrollView->getContentSize().width/4.0);
                    }
                    customScrollView->HideShow(hidePanel);
                }
                    break;
                case MAIN_BUTTON_HIDESHOW_PROPERTIES:
                {
                    bool hidePanel;
                    if (useTouchDownToOpenClosePanel)
                        hidePanel=propertiesPanel->getPositionX()==0;
                    else{
                        if (propertiesPanel->IsVisible())
                            hidePanel=propertiesPanel->getPositionX()<=-propertiesPanel->getContentSize().width/4.0;
                        else
                            hidePanel=(propertiesPanel->getPositionX()-(propertiesPanel->getContentSize().width*PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE))<=-(3.0*propertiesPanel->getContentSize().width/4.0);
                    }
                    HideShowPropertiesPanel(hidePanel);
                }
                    break;
                case DELETE_ITEM:
                    if (type!=Widget::TouchEventType::CANCELED)
                        customPanel->RemoveControl(propertiesPanel->GetSelectedItem());
                    break;
                default:
                    break;
            }
            break;
        default:
            // TODO
            break;
    }
}

template void MainScene::OnStartDragging<ItemSlider>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemPad>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemKnob>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemSwitch>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemKeyboard>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemMultipad>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemWheel>(Vec2 dragStartPoint);
template void MainScene::OnEndDragging<ItemSlider>();
template void MainScene::OnEndDragging<ItemPad>();
template void MainScene::OnEndDragging<ItemKnob>();
template void MainScene::OnEndDragging<ItemSwitch>();
template void MainScene::OnEndDragging<ItemKeyboard>();
template void MainScene::OnEndDragging<ItemMultipad>();
template void MainScene::OnEndDragging<ItemWheel>();
template void MainScene::OnDragging<ItemSlider>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemPad>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemKnob>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemSwitch>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemKeyboard>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemMultipad>(cocos2d::Rect r);
template void MainScene::OnDragging<ItemWheel>(cocos2d::Rect r);

