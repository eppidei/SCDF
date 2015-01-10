
#include "SCDFCDefinitions.h"
#include "SCDFCScrollView.h"
#include "SCDFCWorkingPanel.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "ControlUnit.h"
//#include "PlatformInfo.h"

using namespace SCDFC;
USING_NS_CC;
using namespace ui;

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

void MainScene::SnapToGrid(Rect &r)
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
    draggingImage=ImageView::create("CloseSelected.png");
    //draggingImage->setScale9Enabled(true);
    draggingImage->ignoreContentAdaptWithSize(false);
    draggingImage->setAnchorPoint(Vec2(0,1));
    float width=ItemType::GetSize().width;
    float height=ItemType::GetSize().height;
    draggingImage->setContentSize(Size(width*GetGridDistance(),height*GetGridDistance()));
    draggingImage->setOpacity(50);
    draggingImage->setPosition(dragStartPoint);
    addChild(draggingImage);
}

void MainScene::OnDragging(Rect r)
{
    if (NULL==customPanel.get()) return;
    draggingImage->setPosition(r.origin);
    Vec2 coord(convertToNodeSpace(customPanel->getPosition()));
    Rect workingSpaceRect(coord.x, coord.y,customPanel->getContentSize().width, customPanel->getContentSize().height);
    Rect rr=Rect::ZERO;
    
    float scaledHeight=draggingImage->getContentSize().height;
    float scaledWidth=draggingImage->getContentSize().width;
    if (r.origin.y<=workingSpaceRect.origin.y && (r.origin.x>=workingSpaceRect.origin.x)
        && (r.origin.x+scaledWidth)<=workingSpaceRect.size.width
        && (r.origin.y-scaledHeight)>=workingSpaceRect.origin.y-workingSpaceRect.size.height)
    {
        SnapToGrid(r);
        draggingImage->setPosition(r.origin);
        r.size=draggingImage->getContentSize();
        rr=r;
    }
    customPanel->SetDraggingRect(rr);
}

template <class ItemType> void MainScene::OnEndDragging()
{
    if (NULL==customPanel.get()) return;
    customPanel->CheckAddControl(ItemType::GetID());
    customPanel->SetDraggingRect(Rect::ZERO);
    removeChild(draggingImage);
    draggingImage=NULL;
}

void MainScene::AttachItem(ItemBase *item)
{
    item->Attach(propertiesPanel.get());
}

void MainScene::DetachItem(ItemBase *item)
{
    item->Detach(propertiesPanel.get());
}

void MainScene::EnableScrollView(bool enable)
{
    if (enable)
        customScrollView->setDirection(ScrollView::Direction::VERTICAL);
    else
        customScrollView->setDirection(ScrollView::Direction::NONE);
}

void MainScene::AddToolbar(Rect r)
{
    auto toolbar=Layout::create();
    toolbar->setContentSize(r.size);
    toolbar->setAnchorPoint(Vec2(0,1));
    toolbar->setPosition(r.origin);
    toolbar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    toolbar->setBackGroundColor(Color3B(50,50,50));
    this->addChild(toolbar, -1, ID_TOOLBAR);
    
//    auto buttonShow = Button::create();
//    buttonShow->setTouchEnabled(true);
//    buttonShow->setScale9Enabled(true);
//    buttonShow->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
//    buttonShow->setAnchorPoint(Vec2(0,1));
//    buttonShow->setPosition(Vec2(r.origin.x, toolbar->getContentSize().height));
//    buttonShow->setContentSize(Size(r.size.height, r.size.height));
//    buttonShow->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
//    button->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
    
    int buttonWidth = r.size.height;
    int marginOffset = 0;/*toolbar->getContentSize().height/8*/
    int marginLeft = 24;
    
    auto buttonPanel = CheckBox::create();
    buttonPanel->setTouchEnabled(true);
    buttonPanel->loadTextures("TogglePannelOff.png",
                              "TogglePannelOn.png",
                              "TogglePannelOn.png",
                              "TogglePannelOn.png",
                              "TogglePannelOff.png");
    buttonPanel->ignoreContentAdaptWithSize(false);
    buttonPanel->setAnchorPoint(Vec2(0,1));
    buttonPanel->setPosition(Vec2(marginLeft, toolbar->getContentSize().height-marginOffset));
    buttonPanel->setContentSize(Size(r.size.height, r.size.height-2*marginOffset));
    buttonPanel->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));

   
    auto buttonShowItems = CheckBox::create();
    buttonShowItems->setTouchEnabled(true);
    buttonShowItems->loadTextures("ToggleItemsOff.png",
                                  "ToggleItemsOn.png",
                                  "ToggleItemsOn.png",
                                  "ToggleItemsOn.png",
                                  "ToggleItemsOff.png");
    buttonShowItems->ignoreContentAdaptWithSize(false);
    buttonShowItems->setAnchorPoint(Vec2(0,1));
    buttonShowItems->setPosition(Vec2(buttonPanel->getPosition().x+buttonWidth, toolbar->getContentSize().height- marginOffset));
    buttonShowItems->setContentSize(Size(r.size.height, r.size.height-2*marginOffset));
    buttonShowItems->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
    
    
    //int buttonsWidth = 87;
    auto buttonGrid = Button::create();
    buttonGrid->setTouchEnabled(true);
    //buttonGrid->Sets
    //buttonGrid->setScale9Enabled(true);
    buttonGrid->ignoreContentAdaptWithSize(false);
    buttonGrid->loadTextures("ButtonGrid.png", "ButtonGridOverlay.png", "ButtonGridOverlay.png");
    buttonGrid->setAnchorPoint(Vec2(0,1));
    buttonGrid->setPosition(Vec2(buttonShowItems->getPosition().x+buttonWidth + marginLeft, toolbar->getContentSize().height));
    buttonGrid->setContentSize(Size(buttonWidth, r.size.height));
    buttonGrid->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));

    auto buttonEdit = CheckBox::create();
    buttonEdit->setTouchEnabled(true);
    buttonEdit->loadTextures("ToggleEditOff.png",
                                  "ToggleEditOn.png",
                                  "ToggleEditOn.png",
                                  "ToggleEditOn.png",
                                  "ToggleEditOff.png");
     buttonEdit->ignoreContentAdaptWithSize(false);
    //buttonEdit->setScale9Enabled(true);
    //buttonEdit->loadTextures("ButtonGrid.png", "ButtonGridOverlay.png", "ButtonGridOverlay.png");
    buttonEdit->setAnchorPoint(Vec2(0,1));
    buttonEdit->setPosition(Vec2(buttonGrid->getPosition().x+buttonWidth +marginLeft, toolbar->getContentSize().height));
    buttonEdit->setContentSize(Size(buttonWidth, r.size.height));
    buttonEdit->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
    
    
    //toolbar->addChild(buttonShow,0,TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
    toolbar->addChild(buttonShowItems,0,TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW);
    toolbar->addChild(buttonGrid,0,TOOLBAR_BUTTON_GRID);
    toolbar->addChild(buttonEdit,0,TOOLBAR_BUTTON_ACTIVATE);
    toolbar->addChild(buttonPanel,0,TOOLBAR_BUTTON_HIDESHOW_PROPERTIES);
}

void MainScene::CalculateGrid()
{
    gridUnity.clear();
    Size s=customPanel->getContentSize();
    float divisore=10;
    float width=s.width;
    float height=s.height;
    for (;divisore<1000;){
        if ((0==std::fmod(width,divisore)) && (0==std::fmod(height,divisore)))
            gridUnity.push_back(divisore);
        divisore=divisore+0.5f;
    }
    if (0==gridUnity.size())
        gridUnity.push_back(GetUnityBase());
}

int MainScene::GetUnityBase()
{
//    if (0==gridUnity.size()){
//        printf("Error creating grid\n");
//        exit(EXIT_FAILURE);
//    }
    return 16;//gridUnity[0];
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
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    gridIndex=0;
    draggingImage=NULL;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    

    
#define SCROLLVIEW_WIDTH (4*GetUnityBase())
#define PROPERTIES_WIDTH  4*SCROLLVIEW_WIDTH
#define TOOLBAR_HEIGHT   (SCROLLVIEW_WIDTH/2)
    
    Rect toolbarPanelsize(0,
                          getContentSize().height,
                          getContentSize().width,
                          TOOLBAR_HEIGHT);
    Rect workingPanelsize(0,
                          getContentSize().height,
                          getContentSize().width,
                          getContentSize().height);
    Rect scrollViewect(getContentSize().width,
                       getContentSize().height-toolbarPanelsize.size.height,
                       SCROLLVIEW_WIDTH,
                       getContentSize().height-toolbarPanelsize.size.height);
    Rect propertiesRect(0-PROPERTIES_WIDTH,
                       getContentSize().height-toolbarPanelsize.size.height,
                       PROPERTIES_WIDTH,
                       getContentSize().height-toolbarPanelsize.size.height);
    customScrollView.reset(ItemScrollView::CreateCustomScrollView((MainScene*)this, scrollViewect));
    customPanel.reset(WorkingPanel::CreateCustomPanel((MainScene*)this,workingPanelsize));
    propertiesPanel.reset(PropertiesPanel::CreatePropertiesPanel((MainScene*)this,propertiesRect));
    
    AddToolbar(toolbarPanelsize);
    CalculateGrid();
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
//    uButton->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, this));
    this->addChild(uButton);*/
    
    return true;
}

void MainScene::HideShowScrollview()
{
    MoveTo *actScrollview;
    CallFunc *callback=nullptr;
    if (customScrollView->getPositionX()!=getContentSize().width)
    {
        //Hide
        //customPanel->setContentSize(Size(getContentSize().width, getContentSize().height-TOOLBAR_HEIGHT));
     //   getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width, TOOLBAR_HEIGHT));
       // CalculateGrid();
        actScrollview = MoveTo::create(0.1f, Point(getContentSize().width, getContentSize().height-TOOLBAR_HEIGHT));
    }
    else
    {
        //Show
        actScrollview = MoveTo::create(0.1f, Point(getContentSize().width-SCROLLVIEW_WIDTH, getContentSize().height-TOOLBAR_HEIGHT));
//        callback = CallFunc::create([this](){
//            //customPanel->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, getContentSize().height-TOOLBAR_HEIGHT));
//            getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, TOOLBAR_HEIGHT));
//       //     CalculateGrid();
//        });
    }
//    if (nullptr!=callback){
//        auto seq = Sequence::create(actScrollview, callback, NULL);
//        customScrollView->runAction(seq);
//    }
//    else
        customScrollView->runAction(actScrollview);
}

void MainScene::HideShowPropertiesPanel()
{
    MoveTo *actScrollview;
    CallFunc *callback=nullptr;
    if (propertiesPanel->getPositionX()==0)
    {
        //Hide
        //customPanel->setContentSize(Size(getContentSize().width, getContentSize().height-TOOLBAR_HEIGHT));
        //getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width, TOOLBAR_HEIGHT));
        // CalculateGrid();
        actScrollview = MoveTo::create(0.1f, Point(-PROPERTIES_WIDTH, getContentSize().height-TOOLBAR_HEIGHT));
    }
    else
    {
        //Show
        actScrollview = MoveTo::create(0.1f, Point(0, getContentSize().height-TOOLBAR_HEIGHT));
//        callback = CallFunc::create([this](){
//            //customPanel->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, getContentSize().height-TOOLBAR_HEIGHT));
//            getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, TOOLBAR_HEIGHT));
//            //     CalculateGrid();
//        });
    }
//    if (nullptr!=callback){
//        auto seq = Sequence::create(actScrollview, callback, NULL);
//        customScrollView->runAction(seq);
//    }
//    else
        propertiesPanel->runAction(actScrollview);
}

void MainScene::HideShowToolbar()
{
    MoveTo *actToolbar;
    CallFunc *callback=nullptr;
    Node *toolbar=getChildByTag(ID_TOOLBAR);
    if (toolbar->getPositionY()==getContentSize().height)
    {
        
        //customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height));
     //   CalculateGrid();
        Node *tB=toolbar->getChildByTag(TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
        if (NULL==tB) return;
        tB->retain();
        toolbar->removeChild(tB);
        addChild(tB, 0, TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
        tB->setPosition(Vec2(0, customPanel->getContentSize().height));
        actToolbar = MoveTo::create(0.1f, Point(0, getContentSize().height+TOOLBAR_HEIGHT));
    }
    else
    {
        actToolbar = MoveTo::create(0.1f, Point(0, getContentSize().height));
        callback = CallFunc::create([this,toolbar](){
         //   customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height-TOOLBAR_HEIGHT));
        //    CalculateGrid();
            Node *tB=getChildByTag(TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
            if (NULL==tB) return;
            tB->retain();
            removeChild(tB);
            tB->setPosition(Vec2(0, toolbar->getContentSize().height));
            toolbar->addChild(tB, 0, TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
        });
    }
    if (nullptr!=callback){
        auto seq = Sequence::create(actToolbar, callback, NULL);
        toolbar->runAction(seq);
    }
    else
        toolbar->runAction(actToolbar);
}

void MainScene::OnGridButtonClick()
{
    gridIndex=++gridIndex%gridUnity.size();
}

int position=0;

void MainScene::touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Node* button = dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (TOOLBAR_BUTTON_GRID==button->getTag())
                OnGridButtonClick();
            else if (TOOLBAR_BUTTON_ACTIVATE==button->getTag())
                customPanel->ToggleActiveState();
            else if (TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW==button->getTag())
                HideShowScrollview();
            else if (TOOLBAR_BUTTON_HIDESHOW_TOOLBAR==button->getTag())
                HideShowToolbar();
            else if (TOOLBAR_BUTTON_HIDESHOW_PROPERTIES==button->getTag())
                HideShowPropertiesPanel();
            break;
        case Widget::TouchEventType::MOVED:
            // TODO
          //  break;
        case Widget::TouchEventType::ENDED:
            // TODO
           // break;
        case Widget::TouchEventType::CANCELED:
            // TODO
           // break;
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
template void MainScene::OnStartDragging<ItemSensor1>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<ItemKeyboard>(Vec2 dragStartPoint);
template void MainScene::OnEndDragging<ItemSlider>();
template void MainScene::OnEndDragging<ItemPad>();
template void MainScene::OnEndDragging<ItemKnob>();
template void MainScene::OnEndDragging<ItemSensor1>();
template void MainScene::OnEndDragging<ItemKeyboard>();

