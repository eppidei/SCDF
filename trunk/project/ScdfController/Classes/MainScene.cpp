
#include "SCDFCDefinitions.h"
#include "SCDFCScrollView.h"
#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "MultiSender.h"
#include "LoadSavePanel.h"

//#include "PlatformInfo.h"

using namespace ScdfCtrl;
USING_NS_CC;
using namespace ui;

int MainScene::gridIndex=0;
std::vector<float> MainScene::gridUnity;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
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

    Vec2 coord(convertToNodeSpace(customPanel->getPosition()));
    cocos2d::Rect workingSpaceRect(fmax(0, customPanel->getPositionX()), fmin(customPanel->getPositionY(), getContentSize().height), fmax(getContentSize().width, getContentSize().width-customPanel->getPositionX()), fmin(getContentSize().height,customPanel->getPositionY()));
    cocos2d::Rect rr=cocos2d::Rect::ZERO;
    
    float scaledHeight=ItemType::GetBaseSize().height*GetGridDistance()+ITEMS_LABEL_HEIGHT; //item label
    if (ItemType::ID()!=ITEM_KNOB_ID && ItemType::ID()!=ITEM_KEYBOARD_ID && ItemType::ID()!=ITEM_SWITCH_ID)
        scaledHeight+=ITEMS_LABEL_HEIGHT;   //item control icon on top
    float scaledWidth=ItemType::GetBaseSize().width*GetGridDistance();

    if (r.origin.y<=workingSpaceRect.origin.y && (r.origin.x>=workingSpaceRect.origin.x)
        && (r.origin.x+scaledWidth)<=workingSpaceRect.size.width
        && (r.origin.y-scaledHeight)>=workingSpaceRect.origin.y-workingSpaceRect.size.height)
    {
        r.origin=customPanel->convertToNodeSpace(r.origin);
        SnapToGrid(r);
        r.size=cocos2d::Size(scaledWidth, scaledHeight);
        rr=r;
    }
    customPanel->DetectCollisions(rr);
    customPanel->SetDraggingRect(rr);
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

void MainScene::AddToolbar(cocos2d::Rect r)
{
//    auto toolbar=Layout::create();
//    toolbar->setContentSize(r.size);
//    toolbar->setAnchorPoint(Vec2(0,1));
//    toolbar->setPosition(r.origin);
//    toolbar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
//    toolbar->setBackGroundColor(Color3B(50,50,50));
//  //  toolbar->setClippingEnabled(true);
//    //toolbar->setClippingType(cocos2d::ui::Layout::ClippingType::SCISSOR);
//    toolbar->setOpacity(100);
//    this->addChild(toolbar, 10, ID_TOOLBAR);
//    
////    auto buttonShow = Button::create();
////    buttonShow->setTouchEnabled(true);
////    buttonShow->setScale9Enabled(true);
////    buttonShow->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
////    buttonShow->setAnchorPoint(Vec2(0,1));
////    buttonShow->setPosition(Vec2(r.origin.x, toolbar->getContentSize().height));
////    buttonShow->setContentSize(Size(r.size.height, r.size.height));
////    buttonShow->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
////    button->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
//    
//    int buttonWidth = r.size.height;
//    int marginOffset = 0;/*toolbar->getContentSize().height/8*/
//    int marginLeft = 24;
//    
//    auto buttonPanel = CheckBox::create();
//    buttonPanel->setTouchEnabled(true);
//    buttonPanel->loadTextures("TogglePannelOff.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOff.png");
//    buttonPanel->ignoreContentAdaptWithSize(false);
//    buttonPanel->setAnchorPoint(Vec2(0,1));
//    buttonPanel->setPosition(Vec2(marginLeft, toolbar->getContentSize().height-marginOffset));
//    buttonPanel->setContentSize(cocos2d::Size(r.size.height, r.size.height-2*marginOffset));
//    buttonPanel->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//
//   
////    auto buttonShowItems = CheckBox::create();
////    buttonShowItems->setTouchEnabled(true);
////    buttonShowItems->loadTextures("ToggleItemsOff.png",
////                                  "ToggleItemsOn.png",
////                                  "ToggleItemsOn.png",
////                                  "ToggleItemsOn.png",
////                                  "ToggleItemsOff.png");
////    buttonShowItems->ignoreContentAdaptWithSize(false);
////    buttonShowItems->setAnchorPoint(Vec2(0,1));
////    buttonShowItems->setPosition(Vec2(buttonPanel->getPosition().x+buttonWidth, toolbar->getContentSize().height- marginOffset));
////    buttonShowItems->setContentSize(cocos2d::Size(r.size.height, r.size.height-2*marginOffset));
////    buttonShowItems->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//    
//    
//    //int buttonsWidth = 87;
//    auto buttonGrid = Button::create();
//    buttonGrid->setTouchEnabled(true);
//    //buttonGrid->Sets
//    //buttonGrid->setScale9Enabled(true);
//    buttonGrid->ignoreContentAdaptWithSize(false);
//    buttonGrid->loadTextures("TogglePannelOff.png", "TogglePannelOn.png", "TogglePannelOn.png");
//    buttonGrid->setAnchorPoint(Vec2(0,1));
//    buttonGrid->setPosition(Vec2(buttonPanel->getPosition().x+buttonWidth + marginLeft, toolbar->getContentSize().height));
//    buttonGrid->setContentSize(cocos2d::Size(buttonWidth, r.size.height));
//    buttonGrid->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//
//    auto buttonEdit = CheckBox::create();
//    buttonEdit->setTouchEnabled(true);
//    buttonEdit->loadTextures("ToggleEditOff.png",
//                                  "ToggleEditOn.png",
//                                  "ToggleEditOn.png",
//                                  "ToggleEditOn.png",
//                                  "ToggleEditOff.png");
//     buttonEdit->ignoreContentAdaptWithSize(false);
//    //buttonEdit->setScale9Enabled(true);
//    //buttonEdit->loadTextures("ButtonGrid.png", "ButtonGridOverlay.png", "ButtonGridOverlay.png");
//    buttonEdit->setAnchorPoint(Vec2(0,1));
//    buttonEdit->setPosition(Vec2(buttonGrid->getPosition().x+buttonWidth +marginLeft, toolbar->getContentSize().height));
//    buttonEdit->setContentSize(cocos2d::Size(buttonWidth, r.size.height));
//    buttonEdit->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//    
//    
//    auto buttonLoadSavePanel = CheckBox::create();
//    buttonLoadSavePanel->setTouchEnabled(true);
//    buttonLoadSavePanel->loadTextures("TogglePannelOff.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOn.png",
//                              "TogglePannelOff.png");
//    buttonLoadSavePanel->ignoreContentAdaptWithSize(false);
//    buttonLoadSavePanel->setAnchorPoint(Vec2(0,1));
//    buttonLoadSavePanel->setPosition(Vec2(buttonEdit->getPosition().x+buttonWidth +marginLeft, toolbar->getContentSize().height));
//    buttonLoadSavePanel->setContentSize(cocos2d::Size(buttonWidth, r.size.height));
//    buttonLoadSavePanel->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//    
//    toolbar->addChild(buttonLoadSavePanel,0,TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
//   // toolbar->addChild(buttonShowItems,0,TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW);
//   toolbar->addChild(buttonGrid,0,TOOLBAR_BUTTON_GRID);
//    toolbar->addChild(buttonEdit,0,TOOLBAR_BUTTON_ACTIVATE);
//    //toolbar->addChild(buttonPanel,0,TOOLBAR_BUTTON_HIDESHOW_PROPERTIES);
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
//    if (0==gridUnity.size()){
//        printf("Error creating grid\n");
//        exit(EXIT_FAILURE);
//    }
    return 0.015*Director::getInstance()->getVisibleSize().width;
//    return 16.0;//gridUnity[0];
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
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Director::getInstance()->setDisplayStats(false);
    cocos2d::Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    //draggingImage=NULL;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
//    setColor(Color3B::GRAY);
#define SCROLLVIEW_WIDTH (8.0*GetUnityBase())
#define PROPERTIES_WIDTH (18.0*GetUnityBase())
#define TOOLBAR_HEIGHT   (2.0*GetUnityBase())
    
    cocos2d::Rect toolbarPanelsize(0,
                          getContentSize().height,
                          getContentSize().width,
                          TOOLBAR_HEIGHT);
    cocos2d::Rect workingPanelsize(-(getContentSize().width/2.0),
                          3.0*getContentSize().height/2.0,
                          2*getContentSize().width,
                          2*getContentSize().height);
    cocos2d::Rect scrollViewect(getContentSize().width-(SCROLLVIEW_WIDTH*SCROLLVIEW_TONGUE_PERCENTAGE),
                       getContentSize().height-toolbarPanelsize.size.height,
                       SCROLLVIEW_WIDTH,
                       getContentSize().height-toolbarPanelsize.size.height);
    cocos2d::Rect propertiesRect(PROPERTIES_WIDTH*(PROPERTIES_PANEL_TONGUE_PERCENTAGE-1.0),
                       getContentSize().height-toolbarPanelsize.size.height,
                       PROPERTIES_WIDTH,
                       getContentSize().height-toolbarPanelsize.size.height);

    customPanel.reset(WorkingPanel::CreateCustomPanel((MainScene*)this,workingPanelsize));
    
        LOGD("Created custom panel");
    customScrollView.reset(ItemScrollView::CreateCustomScrollView((MainScene*)this, scrollViewect));
    LOGD("Created custom scrollview");


    propertiesPanel.reset(dynamic_cast<PropertiesPanel*>(PanelBase::CreatePanel<PropertiesPanel>((MainScene*)this,propertiesRect)));

//    loadSavePanel.reset(dynamic_cast<LoadSavePanel*>(PanelBase::CreatePanel<LoadSavePanel>((MainScene*)this,propertiesRect)));

   // AddToolbar(toolbarPanelsize);

    LOGD("Added toolbar");

    CalculateGrid(customPanel->getContentSize());

    LOGD("Calculated grid");

    // add a "close" icon to exit the progress. it's an autorelease object
    /*auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MainScene::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1,0);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1,1);

    // add "MainScene" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0,2);
    
    
    auto uButton = ui::Button::create();
    uButton->setTouchEnabled(true);
    uButton->loadTextures("2_knob_anim0000.png", "2_knob_anim0000.png", "");
    uButton->setAnchorPoint(Vec2(0,1));
    uButton->setPosition(Vec2(widgetSize.width-150, widgetSize.height-150));
    uButton->setContentSize(Size(50, 50));
    uButton->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
//  uButton->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
    this->addChild(uButton);*/
    
    HideShowPropertiesPanel(false);
    customScrollView->HideShow(false);
    
    return true;
}

void MainScene::HideShowLoadSavePanel()
{
//    propertiesPanel->HideShow(loadSavePanel.get());
}

void MainScene::HideShowPropertiesPanel(bool hide)
{
    customPanel->SetActive(hide);
    propertiesPanel->HideShow(hide);
    propertiesPanel->Update(NULL, SCDFC_EVENTS_Update);
}

//void MainScene::HideShowToolbar()
//{
//    MoveTo *actToolbar;
//    CallFunc *callback=nullptr;
//    Node *toolbar=getChildByTag(ID_TOOLBAR);
//    if (toolbar->getPositionY()==getContentSize().height)
//    {
//        
//        //customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height));
//     //   CalculateGrid();
//        Node *tB=toolbar->getChildByTag(TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
//        if (NULL==tB) return;
//        tB->retain();
//        toolbar->removeChild(tB);
//        addChild(tB, 0, TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
//        tB->setPosition(Vec2(0, customPanel->getContentSize().height));
//        actToolbar = MoveTo::create(0.1f, cocos2d::Point(0, getContentSize().height+TOOLBAR_HEIGHT));
//    }
//    else
//    {
//        actToolbar = MoveTo::create(0.1f, cocos2d::Point(0, getContentSize().height));
//        callback = CallFunc::create([this,toolbar](){
//         //   customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height-TOOLBAR_HEIGHT));
//        //    CalculateGrid();
//            Node *tB=getChildByTag(TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
//            if (NULL==tB) return;
//            tB->retain();
//            removeChild(tB);
//            tB->setPosition(Vec2(0, toolbar->getContentSize().height));
//            toolbar->addChild(tB, 0, TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
//        });
//    }
//    if (nullptr!=callback){
//        auto seq = Sequence::create(actToolbar, callback, NULL);
//        toolbar->runAction(seq);
//    }
//    else
//        toolbar->runAction(actToolbar);
//}

void MainScene::OnGridButtonClick()
{
    gridIndex=++gridIndex%gridUnity.size();
}

//int position=0;
//
//void ChangeBitmap(Ref *pSender, bool left, bool selected)
//{
//    Button *b=dynamic_cast<Button*>(pSender);
//    if (selected)
//    {
//        if (left)
//        {
//            b->loadTextureNormal("btnPanelLeftClose.png");
//            b->loadTexturePressed("btnPanelLeftClose.png");
//        }
//        else
//        {
//            b->loadTextureNormal("btnPanelRightClose.png");
//            b->loadTexturePressed("btnPanelRightClose.png");
//        }
//    }
//    else
//    {
//        if (left)
//        {
//            b->loadTextureNormal("btnPanelLeftOpen.png");
//            b->loadTexturePressed("btnPanelLeftOpen.png");
//        }
//        else
//        {
//            b->loadTextureNormal("btnPanelRightOpen.png");
//            b->loadTexturePressed("btnPanelRightOpen.png");
//        }
//    }
//}

//void MainScene::SlideWorkingPanel(bool close)
//{
//    float posX=0;
//    if (customPanel->getPositionX()==0 && !close)
//        posX=propertiesPanel->getPositionX()+propertiesPanel->getContentSize().width-32;
//    
//    if (close && customPanel->getPositionX()==0) return;
//    MoveTo *action=MoveTo::create(0.1f, cocos2d::Vec2(posX, customPanel->getPositionY()));
//    CallFunc *callback=nullptr;
//    
//    if (nullptr!=callback){
//        auto seq = Sequence::create(action, callback, NULL);
//        customPanel->runAction(seq);
//    }
//    else
//        customPanel->runAction(action);
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
                    break;
                case MAIN_BUTTON_SAVE:
                {
                    SavePanel *p=SavePanel::create();
                    p->SetWorkingPanel(customPanel.get());
                    addChild(p,100);
                }
                    break;
                case MAIN_BUTTON_LOAD:
                {
                    LoadPanel *p=LoadPanel::create();
                    p->SetWorkingPanel(customPanel.get());
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
                    newPosX=fmin(getContentSize().width-(panel->getContentSize().width*SCROLLVIEW_TONGUE_PERCENTAGE), fmax(getContentSize().width-panel->getContentSize().width, panel->getPositionX()-diff_x));
                    break;
                case MAIN_BUTTON_HIDESHOW_PROPERTIES:
                    panel=propertiesPanel.get();
                    newPosX=fmax(-panel->getContentSize().width+(panel->getContentSize().width*PROPERTIES_PANEL_TONGUE_PERCENTAGE), fmin(0, panel->getPositionX()-diff_x));
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
                            hidePanel=(customScrollView->getPositionX()+(customScrollView->getContentSize().width*SCROLLVIEW_TONGUE_PERCENTAGE))>=(getContentSize().width-customScrollView->getContentSize().width/4.0);
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
                            hidePanel=(propertiesPanel->getPositionX()-(propertiesPanel->getContentSize().width*PROPERTIES_PANEL_TONGUE_PERCENTAGE))<=-(3.0*propertiesPanel->getContentSize().width/4.0);
                    }
                    HideShowPropertiesPanel(hidePanel);
                }
                    break;
                case DELETE_ITEM:
                    if (type!=Widget::TouchEventType::CANCELED)
                        customPanel->CheckRemoveControl(propertiesPanel->GetSelectedItem());
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

void menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
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

