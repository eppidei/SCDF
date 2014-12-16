
#include "SCDFCDefinitions.h"
#include "SCDFCScrollView.h"
#include "SCDFCWorkingPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"

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
    draggingImage->setScale9Enabled(true);
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
    
    float scaledHeight=draggingImage->getContentSize().height;// ItemType::GetSize().width*GetGridDistance();
    float scaledWidth=draggingImage->getContentSize().width;//ItemType::GetSize().height*GetGridDistance();
    if (r.origin.y<=workingSpaceRect.origin.y && (r.origin.x>=workingSpaceRect.origin.x)
        && (r.origin.x+scaledWidth)<=workingSpaceRect.size.width
        && (r.origin.y-scaledHeight)>=workingSpaceRect.origin.y-workingSpaceRect.size.height)
    {
        SnapToGrid(r);
        //draggingImage->setContentSize(rr.size); //basta solo la position?
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
    toolbar->setBackGroundColor(Color3B::MAGENTA);
    this->addChild(toolbar, -1, ID_TOOLBAR);
    auto button = Button::create();
    button->setTouchEnabled(true);
    button->setScale9Enabled(true);
    button->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setPosition(Vec2(r.origin.x, toolbar->getContentSize().height));
    button->setContentSize(Size(r.size.height, r.size.height));
    button->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
    auto button1 = Button::create();
    button1->setTouchEnabled(true);
    button1->setScale9Enabled(true);
    button1->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
    button1->setAnchorPoint(Vec2(0,1));
    button1->setPosition(Vec2(button->getPosition().x+2*button->getContentSize().width, toolbar->getContentSize().height));
    button1->setContentSize(Size(r.size.height, r.size.height));
    button1->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
    auto button2 = Button::create();
    button2->setTouchEnabled(true);
    button2->setScale9Enabled(true);
    button2->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
    button2->setAnchorPoint(Vec2(0,1));
    button2->setPosition(Vec2(button1->getPosition().x+2*button->getContentSize().width, toolbar->getContentSize().height));
    button2->setContentSize(Size(r.size.height, r.size.height));
    button2->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
    auto button3 = Button::create();
    button3->setTouchEnabled(true);
    button3->setScale9Enabled(true);
    button3->loadTextures("CloseNormal.png", "CloseSelected.png", "CloseSelected.png");
    button3->setAnchorPoint(Vec2(0,1));
    button3->setPosition(Vec2(button2->getPosition().x+2*button->getContentSize().width, toolbar->getContentSize().height));
    button3->setContentSize(Size(r.size.height, r.size.height));
    button3->addTouchEventListener(this, toucheventselector(MainScene::touchEvent));
    toolbar->addChild(button,0,TOOLBAR_BUTTON_HIDESHOW_TOOLBAR);
    toolbar->addChild(button1,0,TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW);
    toolbar->addChild(button2,0,TOOLBAR_BUTTON_GRID);
    toolbar->addChild(button3,0,TOOLBAR_BUTTON_ACTIVATE);

    
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
}

int MainScene::GetGridBase()
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
    
#define SCROLLVIEW_WIDTH (8*GetGridBase())
#define TOOLBAR_HEIGHT   (4*GetGridBase())
    
    Rect toolbarPanelsize(0,
                          getContentSize().height,
                          getContentSize().width-SCROLLVIEW_WIDTH,
                          TOOLBAR_HEIGHT);
    Rect workingPanelsize(0,
                          getContentSize().height-toolbarPanelsize.size.height,
                          getContentSize().width-SCROLLVIEW_WIDTH,
                          getContentSize().height-toolbarPanelsize.size.height);
    Rect scrollViewect(workingPanelsize.origin.x+workingPanelsize.size.width,
                       getContentSize().height,
                       getContentSize().width-workingPanelsize.size.width,
                       getContentSize().height);
    customScrollView.reset(SScrollView::CreateCustomScrollView((MainScene*)this, scrollViewect));
    
    customPanel.reset(WorkingPanel::CreateCustomPanel((MainScene*)this,workingPanelsize));
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
        customPanel->setContentSize(Size(getContentSize().width, getContentSize().height-TOOLBAR_HEIGHT));
        getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width, TOOLBAR_HEIGHT));
        CalculateGrid();
        actScrollview = MoveTo::create(0.1f, Point(getContentSize().width, getContentSize().height));
    }
    else
    {
        actScrollview = MoveTo::create(0.1f, Point(getContentSize().width-SCROLLVIEW_WIDTH, getContentSize().height));
        callback = CallFunc::create([this](){
            customPanel->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, getContentSize().height-TOOLBAR_HEIGHT));
            getChildByTag(ID_TOOLBAR)->setContentSize(Size(getContentSize().width-SCROLLVIEW_WIDTH, TOOLBAR_HEIGHT));
            CalculateGrid();
        });
    }
    if (nullptr!=callback){
        auto seq = Sequence::create(actScrollview, callback, NULL);
        customScrollView->runAction(seq);
    }
    else
        customScrollView->runAction(actScrollview);
}

void MainScene::HideShowToolbar()
{
    MoveTo *actToolbar;
    CallFunc *callback=nullptr;
    Node *toolbar=getChildByTag(ID_TOOLBAR);
    if (toolbar->getPositionY()==getContentSize().height)
    {
        
        customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height));
        CalculateGrid();
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
            customPanel->setContentSize(Size(customScrollView->getPositionX(), getContentSize().height-TOOLBAR_HEIGHT));
            CalculateGrid();
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

void MainScene::touchEvent(Ref *pSender, cocos2d::ui::TouchEventType type)
{
    ui::Button* button = dynamic_cast<ui::Button*>(pSender);
    switch (type)
    {
        case (int)Widget::TouchEventType::BEGAN:
            if (TOOLBAR_BUTTON_GRID==button->getTag())
                OnGridButtonClick();
            else if (TOOLBAR_BUTTON_ACTIVATE==button->getTag())
                customPanel->ToggleActiveState();
            else if (TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW==button->getTag())
                HideShowScrollview();
            else if (TOOLBAR_BUTTON_HIDESHOW_TOOLBAR==button->getTag())
                HideShowToolbar();
            break;
        case (int)ui::Widget::TouchEventType::MOVED:
        {
            int diff=button->getTouchMovePosition().y-button->getTouchBeganPosition().y;
            diff/=2;
            if ((position+diff)<0 || (position+diff)>124) break;
            position+=diff;
            char str[50];
            sprintf(str, "2_knob_anim0%03d.png", position);
            button->loadTextures(str, str, "");
        }
            // TODO
            break;
        case (int)ui::Widget::TouchEventType::ENDED:
            // TODO
            break;
        case (int)ui::Widget::TouchEventType::CANCELED:
            // TODO
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
template void MainScene::OnStartDragging<Knob>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<Sensor1>(Vec2 dragStartPoint);
template void MainScene::OnStartDragging<Keyboard>(Vec2 dragStartPoint);
template void MainScene::OnEndDragging<ItemSlider>();
template void MainScene::OnEndDragging<ItemPad>();
template void MainScene::OnEndDragging<Knob>();
template void MainScene::OnEndDragging<Sensor1>();
template void MainScene::OnEndDragging<Keyboard>();

