//
//  SCDFCScollView.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#include "SCDFCDefinitions.h"
#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "MultiSender.h"
#include "LoadSavePanel.h"
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
//#define SCROLLVIEW_ITEM_SIZE        (SCROLLBAR_ITEM_SIDE_BASE*parent->GetUnityBase())
//#define SCROLLVIEW_ITEM_DISTANCE    (SCROLLVIEW_ITEM_SIZE/2)
//#define SCROLLVIEW_ITEM_RECORD      (SCROLLVIEW_ITEM_SIZE+SCROLLVIEW_ITEM_DISTANCE)
    
    int numElements=scrollView->getChildrenCount();
    float innerWidth = scrollView->getContentSize().width;
    
    float itemHeight = scrollView->getContentSize().width*0.4;
    float itemDistance = itemHeight/2;
    float scrollviewRecord=itemHeight+itemDistance;
    
    scrollView->setInnerContainerSize(cocos2d::Size(innerWidth,(numElements+1)*scrollviewRecord));
    float newInnerHeight = scrollView->getInnerContainerSize().height;
    
    Vector<Node*> _childrens = scrollView->getChildren();
    int i=0;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        (*it)->setPosition(Vec2(innerWidth / 2.0f-(*it)->getContentSize().width/2.0f, newInnerHeight -i*itemDistance- ((*it)->getContentSize().height)*i));
        i++;
    }
    Button* button = Button::create();
    button->loadTextureNormal(ItemType::GetIcon());
    button->loadTexturePressed(ItemType::GetIconPressed());
    button->setAnchorPoint(Vec2(0,1));
    button->ignoreContentAdaptWithSize(false);
    //button->setScale9Enabled(true);
    button->setContentSize(cocos2d::Size(itemHeight, itemHeight));
    button->setPosition(Vec2(innerWidth / 2.0f-button->getContentSize().width/2.0f, newInnerHeight -numElements*itemDistance- button->getContentSize().height*numElements));
    
    SubscribeButton(button, /*id==ITEM_KEYBOARD_ID*/true);
    scrollView->addChild(button,0,ItemType::GetID());
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
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
    
    scrollView=cocos2d::ui::ScrollView::create();
    addChild(scrollView);
    scrollView->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    scrollView->setInertiaScrollEnabled(true);
    scrollView->setContentSize(cocos2d::Size(r.size.width-GetBackgroundBitmapLeftOffset(), r.size.height-(GetBackgroundBitmapTopOffset()+GetBackgroundBitmapBottomOffset())));
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
    DoInit(main, r);
    parent->addChild(this,7);
    
    std::vector<std::string> v;
    v.push_back("sliderIcon.png");
    v.push_back("sliderIcon.png");
    v.push_back("sliderIcon.png");
    cocos2d::Rect subPanelRect(0,0,SCROLLBAR_ITEM_SIDE_BASE*parent->GetUnityBase(),SCROLLBAR_ITEM_SIDE_BASE*parent->GetUnityBase()*v.size());
    LOGD("ITEM SCROLL ORIG %f,%f SIZE %f,%f",r.origin.x,r.origin.y,r.size.width,r.size.height);
    
    LOGD("Add slider button");
    AddButtonToScrollView<ItemSlider>();
//    CreateSubpanel(ItemSlider::GetID(),v,subPanelRect);
//    LOGD("Add pad button");
//    v.clear();
//    v.push_back("padIcon.png");
//    v.push_back("padIcon.png");
//    v.push_back("padIcon.png");
    AddButtonToScrollView<ItemPad>();
//    CreateSubpanel(ItemPad::GetID(),v,subPanelRect);
//    LOGD("Add knob button");
//    v.clear();
//    v.push_back("knobIcon.png");
//    v.push_back("knobIcon.png");
//    v.push_back("knobIcon.png");
    AddButtonToScrollView<ItemKnob>();
//    CreateSubpanel(ItemKnob::GetID(),v,subPanelRect);
//    LOGD("Add switch pad button");
//    v.clear();
//    v.push_back("sensorIcon.png");
//    v.push_back("sensorIcon.png");
//    v.push_back("sensorIcon.png");
    AddButtonToScrollView<ItemSwitch>();
//    CreateSubpanel(ItemSwitch::GetID(),v,subPanelRect);
    LOGD("Add keyboard button");
    AddButtonToScrollView<ItemKeyboard>();

    LOGD("Init with content over");
    
    setBackGroundImage("rightPanel.png");
    const int bitmapcapInsetOffset=80;
    cocos2d::Rect rr(58, 33, getBackGroundImageTextureSize().width-58, getBackGroundImageTextureSize().height-33-55);
    setBackGroundImageScale9Enabled(true);
    setBackGroundImageCapInsets(rr);
    
    auto button = Button::create();
    button->loadTextureNormal("btnOFF.png");
    button->loadTexturePressed("btnON.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(30, 60));
    button->setPosition(Vec2(45, getContentSize().height-28));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW);
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
            scrollView->setDirection(ScrollView::Direction::NONE);
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
            float diff_x=button->getTouchMovePosition().x-button->getTouchBeganPosition().x;
            float diff_y=button->getTouchMovePosition().y-button->getTouchBeganPosition().y;
            float newX=dragStartPoint.x+diff_x;
            float newY=dragStartPoint.y+diff_y;
            parent->OnDragging<ItemType>(cocos2d::Rect(newX,newY,button->getContentSize().width, button->getContentSize().height));
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
    ui::Button* button = dynamic_cast<ui::Button*>(pSender);
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

//    switch()
//    {
//        case ITEM_SLIDER_ID:
//            DoDragItemOnTouchEvent<ItemSlider>(type, button);
//            break;
//        case ITEM_PAD_ID:
//            DoDragItemOnTouchEvent<ItemPad>(type, button);
//            break;
//        case ITEM_KEYBOARD_ID:
//            DoDragItemOnTouchEvent<ItemKeyboard>(type, button);
//            break;
//        case ITEM_KNOB_ID:
//            DoDragItemOnTouchEvent<ItemKnob>(type, button);
//            break;
//        case ITEM_SWITCH_ID:
//            DoDragItemOnTouchEvent<ItemSwitch>(type, button);
//            break;
//        default:
//            break;
//    }
}

template void ItemScrollView::AddButtonToScrollView<ItemSlider>();
template void ItemScrollView::AddButtonToScrollView<ItemPad>();
template void ItemScrollView::AddButtonToScrollView<ItemKnob>();
template void ItemScrollView::AddButtonToScrollView<ItemKeyboard>();
template void ItemScrollView::AddButtonToScrollView<ItemSwitch>();
