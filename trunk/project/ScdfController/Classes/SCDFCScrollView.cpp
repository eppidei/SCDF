//
//  SCDFCScollView.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#include "SCDFCDefinitions.h"
#include "SCDFCItems.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "MultiSender.h"

//#include "PlatformInfo.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

void ItemScrollView::SubscribeButton(cocos2d::ui::Button *button, bool drag)
{
    if (drag)
        button->addTouchEventListener(CC_CALLBACK_2(ItemScrollView::DragItemOnTouchEvent, this));
    else
        button->addTouchEventListener(CC_CALLBACK_2(ItemScrollView::ToggleItemMenuOnTouchEvent, this));
}

void ItemScrollViewSubpanel::SubscribeButton(cocos2d::ui::Button *button, bool drag)
{
    button->addTouchEventListener(CC_CALLBACK_2(ItemScrollViewSubpanel::DragItemOnTouchEvent, this));
}

template <class ItemType> void ItemScrollView::AddButtonToScrollView()
{
    int numElements=scrollView->getChildrenCount();
    float innerWidth = scrollView->getContentSize().width;
    
    float itemHeight = scrollView->getContentSize().width;
    float itemDistance = itemHeight/2;
    float scrollviewRecord=itemHeight+itemDistance;
    
    scrollView->setInnerContainerSize(cocos2d::Size(innerWidth,(numElements+1)*scrollviewRecord));
    float newInnerHeight = scrollView->getInnerContainerSize().height;
    
    Vector<Node*> _childrens = scrollView->getChildren();
    int i=0;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        (*it)->setPosition(Vec2(0, newInnerHeight -i*itemDistance- ((*it)->getContentSize().height)*i));
        i++;
    }
    Button* button = Button::create();
    button->loadTextureNormal(ItemType::GetIcon());
    button->loadTexturePressed(ItemType::GetIconPressed());
    button->setAnchorPoint(Vec2(0,1));
    button->ignoreContentAdaptWithSize(false);
    //button->setScale9Enabled(true);
    button->setContentSize(cocos2d::Size(itemHeight, itemHeight));
    button->setPosition(Vec2(0, newInnerHeight -numElements*itemDistance- button->getContentSize().height*numElements));
    
    SubscribeButton(button, /*id==ITEM_KEYBOARD_ID*/true);
    scrollView->addChild(button,0,ItemType::ID());
}

ItemScrollView *ItemScrollView::CreateCustomScrollView(MainScene *main, cocos2d::Rect r)
{
    ItemScrollView *scrollView = (ItemScrollView*)ItemScrollView::create();
    scrollView->InitWithContent(main,r);
    return scrollView;
}

ItemScrollView *ItemScrollViewSubpanel::CreateScrollViewSubPanel(MainScene *main, std::vector<std::string> buttonsBtm, cocos2d::Rect r, int ownerID)
{
    ItemScrollViewSubpanel *scrollView = (ItemScrollViewSubpanel*)ItemScrollViewSubpanel::create();
    scrollView->InitWithContentAndBitmaps(main, r, buttonsBtm, ownerID);
    return scrollView;
}

void ItemScrollView::SetDirection(ScrollView::Direction dir)
{
    scrollView->setDirection(dir);
}

void ItemScrollView::CreateSubpanel(int itemID, std::vector<std::string> bitmaps, cocos2d::Rect r)
{
    if (itemSubpanels.find(itemID)!=itemSubpanels.end())
        parent->removeChild(itemSubpanels[itemID]);
    ItemScrollView *scroll=ItemScrollViewSubpanel::CreateScrollViewSubPanel(parent, bitmaps, r, itemID);
    parent->addChild(scroll,6);
    scroll->setVisible(false);
    scroll->setOpacity(0);
    itemSubpanels[itemID]=scroll;
}

void ItemScrollView::HideAllSubPanels()
{
    for (auto it=itemSubpanels.begin();it!=itemSubpanels.end();++it)
    {
        if (!it->second->isVisible()) continue;
        ToggleShowSubpanel(it->first,0);
    }
}

void ItemScrollView::ToggleShowSubpanel(int itemId, float y)
{
    FadeTo *actScrollview;
    CallFunc *callback=nullptr;
    auto it=itemSubpanels.find(itemId);
    if (it==itemSubpanels.end()) return;

    ItemScrollView *scroll=it->second;
    if (scroll->isVisible())
    {
        actScrollview = FadeOut::create(0.1f);
        callback = CallFunc::create([scroll](){
                scroll->setVisible(false);
        });
        CheckButton(itemId,false);
    }
    else
    {
        HideAllSubPanels();
        scroll->setPosition(Vec2(GetBackgroundBitmapLeftOffset()+getPositionX()-scroll->getContentSize().width, y));
        scroll->setVisible(true);
        actScrollview = FadeIn::create(0.2f);
        CheckButton(itemId,true);
    }

    if (nullptr!=callback){
        auto seq = Sequence::create(actScrollview, callback, NULL);
        scroll->runAction(seq);
    }
    else
        scroll->runAction(actScrollview);
}
        
void ItemScrollView::DoInit(MainScene *main, cocos2d::Rect r)
{
    parent=main;
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    
    scrollView=cocos2d::ui::ScrollView::create();
    addChild(scrollView);
    scrollView->setBackGroundColorType(Layout::BackGroundColorType::NONE);
//    scrollView->setBackGroundColor(Color3B::YELLOW);
    scrollView->setInertiaScrollEnabled(true);
    scrollView->setContentSize(cocos2d::Size(r.size.width-GetBackgroundBitmapLeftOffset(), r.size.height-(GetBackgroundBitmapTopOffset()+GetBackgroundBitmapBottomOffset())-50));
    scrollView->setAnchorPoint(Vec2(0,1));
    scrollView->setPosition(Vec2(GetBackgroundBitmapLeftOffset(),r.size.height-GetBackgroundBitmapTopOffset()));
}

void ItemScrollViewSubpanel::InitWithContentAndBitmaps(MainScene *main, cocos2d::Rect r, std::vector<std::string> buttonsBtm, int ownerID)
{
//    DoInit(main, r);
//    for (int i=0;i<buttonsBtm.size();++i)
//        AddButtonToScrollView(buttonsBtm[i],buttonsBtm[i], ownerID+i);
//    
////    setBackGroundImage("rightPanel.png");
////    const int bitmapcapInsetOffset=80;
////    cocos2d::Rect rr(58, 33, getBackGroundImageTextureSize().width-58, getBackGroundImageTextureSize().height-33-55);
////    setBackGroundImageScale9Enabled(true);
////    setBackGroundImageCapInsets(rr);
//    
//    setBackGroundColor(Color3B::GREEN);
//    setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
//    setBackGroundColorOpacity(150);
}

void ItemScrollView::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    isOpened=false;
    touchEventsEnabled=true;
    DoInit(main, r);
    parent->addChild(this,7);
    
    const float bitmapTopTransparencyPercentage=0.0096;
    const float bitmapBottomTransparencyPercentage=0.025;
    const float bitmapToolbarHeightPercentage=0.056;
    const float bitmapToolbarWidthPercentage=1.0-SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE;
    const float tongueTopTransparencyPercentage=0.033;

    float ypadding=(getContentSize().height*bitmapToolbarHeightPercentage)*0.1;
    float xpadding=(getContentSize().width*bitmapToolbarWidthPercentage)*0.1;
    
    setBackGroundImage("panelRight.png");
    cocos2d::Rect rr(0, 0, getBackGroundImageTextureSize().width, getBackGroundImageTextureSize().height);
    setBackGroundImageScale9Enabled(true);
    setBackGroundImageCapInsets(rr);
    
    float scrollbarWidth=getContentSize().width*bitmapToolbarWidthPercentage-2.0*xpadding;
    
    float buttonYPos=getContentSize().height*(1.0-bitmapTopTransparencyPercentage) - ypadding;
    float buttonDeleteHeight=scrollbarWidth;

    float scrollbarHeight=(buttonYPos-1.2*buttonDeleteHeight)-getContentSize().height*bitmapBottomTransparencyPercentage;
    
    scrollView->setContentSize(cocos2d::Size(scrollbarWidth, scrollbarHeight));
    scrollView->setPosition(Vec2(getContentSize().width*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE+xpadding,buttonYPos-1.1*buttonDeleteHeight));

    //SAME CODE OF PROPERTIES PANEL TONGUE
    float buttonPlaceholder=(bitmapToolbarWidthPercentage*getContentSize().width)/4.0;
    float arrowButtonDim=0.7*buttonPlaceholder;
    const float tongueBiggerFactor=3.0;
    cocos2d::Size arrowButtonSize=cocos2d::Size(1.22*arrowButtonDim*tongueBiggerFactor, 1.26*arrowButtonDim*tongueBiggerFactor);
    ////////////////
    
    auto button = Button::create();
    button->loadTextureNormal("rightPanelTongueClosed.png");
    button->loadTexturePressed("rightPanelTongueClosed.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(arrowButtonSize);
    button->setPosition(Vec2(getContentSize().width*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE-arrowButtonSize.width, getContentSize().height*(1-bitmapTopTransparencyPercentage)+(tongueTopTransparencyPercentage*arrowButtonSize.height)));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_HIDESHOW_SCROLLVIEW);
    
    button = Button::create();
    button->loadTextureNormal("deleteDefault.png");
    button->loadTexturePressed("deleteHover.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(scrollbarWidth, buttonDeleteHeight));
    button->setPosition(Vec2(getContentSize().width*(1+SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE)/2.0-button->getContentSize().width/2.0, buttonYPos));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,DELETE_ITEM);
    
    AddButtonToScrollView<ItemSlider>();
    AddButtonToScrollView<ItemPad>();
    AddButtonToScrollView<ItemKnob>();
    AddButtonToScrollView<ItemSwitch>();
    AddButtonToScrollView<ItemWheel>();
    AddButtonToScrollView<ItemKeyboard>();
}


float ItemScrollView::RetrieveButtonYCoordInScrollview(ui::Button* button)
{
    cocos2d::Rect r=scrollView->getInnerContainer()->getBoundingBox();
    return button->getPosition().y+r.origin.y;
}

template <class ItemType> void ItemScrollView::DoDragItemOnTouchEvent(cocos2d::ui::Widget::TouchEventType type, Button *button)
{
    static Vec2 dragStartPoint;
    cocos2d::Rect buttonRect(button->getPositionX(), RetrieveButtonYCoordInScrollview(button), button->getContentSize().width, button->getContentSize().height);
    
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
        {
            button->loadTextureNormal(ItemType::GetIconPressed());
            Vec2 parentScreenCoord=/*parent->*/convertToWorldSpace(scrollView->getPosition());
            dragStartPoint=Vec2(parentScreenCoord.x+buttonRect.origin.x,parentScreenCoord.y-(scrollView->getContentSize().height-buttonRect.origin.y));
            int diffX=ItemType::GetBaseSize().width*parent->GetGridDistance()-buttonRect.size.width;
            int diffY=ItemType::GetBaseSize().height*parent->GetGridDistance()-buttonRect.size.height;
            dragStartPoint.x=dragStartPoint.x-(diffX/2.0f);
            dragStartPoint.y=dragStartPoint.y+(diffY/2.0f);
            parent->OnStartDragging<ItemType>(dragStartPoint);
            break;
        }
        case Widget::TouchEventType::MOVED:
        {
            scrollView->setDirection(ScrollView::Direction::NONE);
            float diff_x=button->getTouchMovePosition().x-button->getTouchBeganPosition().x;
            float diff_y=button->getTouchMovePosition().y-button->getTouchBeganPosition().y;
            float newX=dragStartPoint.x+diff_x;
            float newY=dragStartPoint.y+diff_y;
            parent->OnDragging<ItemType>(cocos2d::Rect(newX,newY,button->getContentSize().width, button->getContentSize().height+ITEMS_LABEL_HEIGHT));
        }
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            button->loadTextureNormal(ItemType::GetIcon());
            scrollView->setDirection(ScrollView::Direction::VERTICAL);
            parent->OnEndDragging<ItemType>();
            break;
        default:
            break;
    }
}
void ItemScrollView::DragItemOnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Vec2 parentScreenCoord=/*parent->*/convertToWorldSpace(scrollView->getPosition());
    ui::Button* button = dynamic_cast<ui::Button*>(pSender);
    if (type==Widget::TouchEventType::MOVED && button->getTouchMovePosition().x>=parentScreenCoord.x) return;
    switch(button->getTag())
    {
        case ITEM_SLIDER_ID:
            DoDragItemOnTouchEvent<ItemSlider>(type, button);
            break;
        case ITEM_PAD_ID:
            DoDragItemOnTouchEvent<ItemPad>(type, button);
            break;
        case ITEM_KEYBOARD_ID:
            DoDragItemOnTouchEvent<ItemKeyboard>(type, button);
            break;
        case ITEM_KNOB_ID:
            DoDragItemOnTouchEvent<ItemKnob>(type, button);
            break;
        case ITEM_SWITCH_ID:
            DoDragItemOnTouchEvent<ItemSwitch>(type, button);
            break;
        case ITEM_WHEEL_ID:
            DoDragItemOnTouchEvent<ItemWheel>(type, button);
            break;
        default:
            break;
    }
}

void ItemScrollView::CheckButton(int buttonID, bool check)
{
    ui::Button* button = dynamic_cast<ui::Button*>(getChildByTag(buttonID));
    if (NULL==button) return;
    std::string normal, pressed;
    switch(button->getTag())
    {
        case ITEM_SLIDER_ID:
            pressed="sliderIcon.png";
            normal="sliderIcon.png";
            if (check)
                normal="sliderIcon.png";
            break;
        case ITEM_PAD_ID:
            pressed="padIcon.png";
            normal="padIcon.png";
            if (check)
                normal="padIcon.png";
            break;
//        case ITEM_KEYBOARD_ID:
//            pressed=
//            normal=;
//            if (check)
//                normal=;
//            break;
        case ITEM_KNOB_ID:
            pressed="knobIcon.png";
            normal="knobIcon.png";
            if (check)
                normal="knobIcon.png";
            break;
        case ITEM_SWITCH_ID:
            pressed="sensorIcon.png";
            normal="sensorIcon.png";
            if (check)
                normal="sensorIcon.png";
            break;
        default:
            return;
    }
    button->loadTextureNormal(normal);
    button->loadTexturePressed(pressed);
}

void ItemScrollView::ToggleItemMenuOnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    ui::Button* button = dynamic_cast<ui::Button*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
        {
            Vec2 temp=scrollView->convertToWorldSpace(Vec2(0,button->getPositionY()));
            ToggleShowSubpanel(button->getTag(),/*convertToNodeSpace(temp).y*/temp.y);
        }
        default:
            break;
    }
}

void ItemScrollView::HideShow(bool hide)
{
    if (!touchEventsEnabled) return;
    
    EnableTouchEvents(false);
    MoveTo *actScrollview;
    
    CallFunc *callback=CallFunc::create([this](){
            this->EnableTouchEvents(true);
        });
    
    if (hide)
    {
        HideAllSubPanels();
        actScrollview = MoveTo::create(0.1f, cocos2d::Point(getParent()->getContentSize().width-(getContentSize().width*SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE), getPositionY()));
        dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_HIDESHOW_SCROLLVIEW))->loadTextureNormal("rightPanelTongueClosed.png");
        dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_HIDESHOW_SCROLLVIEW))->loadTexturePressed("rightPanelTongueClosed.png");
        isOpened=false;
    }
    else
    {
        actScrollview = MoveTo::create(0.1f, cocos2d::Point(getParent()->getContentSize().width-getContentSize().width, getPositionY()));
        dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_HIDESHOW_SCROLLVIEW))->loadTextureNormal("rightPanelTongueOpened.png");
        dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_HIDESHOW_SCROLLVIEW))->loadTexturePressed("rightPanelTongueOpened.png");
        isOpened=true;
    }
    auto seq = Sequence::create(actScrollview, callback, NULL);
    runAction(seq);
//    runAction(actScrollview);
}
template void ItemScrollView::AddButtonToScrollView<ItemSlider>();
template void ItemScrollView::AddButtonToScrollView<ItemPad>();
template void ItemScrollView::AddButtonToScrollView<ItemKnob>();
template void ItemScrollView::AddButtonToScrollView<ItemKeyboard>();
template void ItemScrollView::AddButtonToScrollView<ItemSwitch>();
template void ItemScrollView::AddButtonToScrollView<ItemWheel>();
