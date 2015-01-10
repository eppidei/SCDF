//
//  SCDFCScollView.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#include "SCDFCDefinitions.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "ControlUnit.h"
//#include "PlatformInfo.h"

using namespace SCDFC;
using namespace cocos2d;
using namespace ui;

template <class ItemType> void ItemScrollView::AddButtonToScrollView(std::string image)
{
#define SCROLLVIEW_ITEM_SIZE        (SCROLLBAR_ITEM_SIDE_BASE*parent->GetUnityBase())
#define SCROLLVIEW_ITEM_DISTANCE    (SCROLLVIEW_ITEM_SIZE/2)
#define SCROLLVIEW_ITEM_RECORD      (SCROLLVIEW_ITEM_SIZE+SCROLLVIEW_ITEM_DISTANCE)
    
    int numElements=getChildrenCount();
    float innerWidth = getContentSize().width;
    setContentSize(Size(innerWidth, MAX(parent->getContentSize().height, (numElements+1)*SCROLLVIEW_ITEM_RECORD)));
    setInnerContainerSize(Size(innerWidth,(numElements+1)*SCROLLVIEW_ITEM_RECORD));
    float newInnerHeight = getInnerContainerSize().height;
    for (int i=0;i<numElements;++i)
    {
        Node *n=getChildByTag(ITEMS_ID_BASE+1+i);
        if (NULL==n) continue;
        n->setPosition(Vec2(innerWidth / 2.0f-n->getContentSize().width/2.0f, newInnerHeight -i*SCROLLVIEW_ITEM_DISTANCE- (n->getContentSize().height)*i));
    }
    Button* button = Button::create();
    button->loadTextureNormal(image);
    button->setAnchorPoint(Vec2(0,1));
    button->ignoreContentAdaptWithSize(false);
    //button->setScale9Enabled(true);
    button->setContentSize(Size(SCROLLVIEW_ITEM_SIZE, SCROLLVIEW_ITEM_SIZE));
    button->setPosition(Vec2(innerWidth / 2.0f-button->getContentSize().width/2.0f, newInnerHeight -numElements*SCROLLVIEW_ITEM_DISTANCE- button->getContentSize().height*numElements));
    button->addTouchEventListener(this, toucheventselector(ItemScrollView::DragItemOnTouchEvent));
    addChild(button,0,ItemType::GetID());
}

ItemScrollView *ItemScrollView::CreateCustomScrollView(MainScene *main, Rect r)
{
    ItemScrollView *scrollView=(ItemScrollView*)ItemScrollView::create();
    scrollView->InitWithContent(main,r);
    return scrollView;
}

void ItemScrollView::InitWithContent(MainScene *main,Rect r)
{
    parent=main;
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    //setBackGroundColor(Color3B::YELLOW, Color3B::RED);
    setBackGroundColor(Color3B(50,50,50));
    setBounceEnabled(true);
    setInertiaScrollEnabled(true);
    parent->addChild(this);
    
    AddButtonToScrollView<ItemSlider>("sliderIcon.png");
    AddButtonToScrollView<ItemPad>("padIcon.png");
    AddButtonToScrollView<ItemKnob>("knobIcon.png");
    AddButtonToScrollView<ItemKeyboard>("fullPad.png");
    AddButtonToScrollView<ItemSensor1>("sensorIcon.png");
}

float ItemScrollView::RetrieveButtonYCoordInScrollview(ui::Button* button)
{
    Rect r=getInnerContainer()->getBoundingBox();
    return button->getPosition().y+r.origin.y;
}

template <class ItemType> void ItemScrollView::DoDragItemOnTouchEvent(cocos2d::ui::TouchEventType type, Button *button)
{
    static Vec2 dragStartPoint;
    Rect buttonRect(button->getPositionX(), RetrieveButtonYCoordInScrollview(button), button->getContentSize().width, button->getContentSize().height);
    
    switch (type)
    {
        case (int)Widget::TouchEventType::BEGAN:
        {
            setDirection(ScrollView::Direction::NONE);
            Vec2 parentScreenCoord=parent->convertToWorldSpace(getPosition());
            dragStartPoint=Vec2(parentScreenCoord.x+buttonRect.origin.x,parentScreenCoord.y-(getContentSize().height-buttonRect.origin.y));
            int diffX=ItemType::GetSize().width*parent->GetGridDistance()-buttonRect.size.width;
            int diffY=ItemType::GetSize().height*parent->GetGridDistance()-buttonRect.size.height;
            dragStartPoint.x=dragStartPoint.x-(diffX/2.0f);
            dragStartPoint.y=dragStartPoint.y+(diffY/2.0f);
            parent->OnStartDragging<ItemType>(dragStartPoint);
            break;
        }
        case (int)ui::Widget::TouchEventType::MOVED:
        {
            float diff_x=button->getTouchMovePosition().x-button->getTouchBeganPosition().x;
            float diff_y=button->getTouchMovePosition().y-button->getTouchBeganPosition().y;
            float newX=dragStartPoint.x+diff_x;
            float newY=dragStartPoint.y+diff_y;
            parent->OnDragging(Rect(newX,newY,button->getContentSize().width, button->getContentSize().height));
        }
            break;
        case (int)ui::Widget::TouchEventType::ENDED:
        case (int)ui::Widget::TouchEventType::CANCELED:
            setDirection(ScrollView::Direction::VERTICAL);
            parent->OnEndDragging<ItemType>();
            break;
        default:
            break;
    }
}

void ItemScrollView::DragItemOnTouchEvent(Ref *pSender, cocos2d::ui::TouchEventType type)
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
        case ITEM_SENSOR1_ID:
            DoDragItemOnTouchEvent<ItemSensor1>(type, button);
            break;
        default:
            break;
    }
}

template void ItemScrollView::AddButtonToScrollView<ItemSlider>(std::string image);
template void ItemScrollView::AddButtonToScrollView<ItemPad>(std::string image);
template void ItemScrollView::AddButtonToScrollView<ItemKnob>(std::string image);
template void ItemScrollView::AddButtonToScrollView<ItemSensor1>(std::string image);
template void ItemScrollView::AddButtonToScrollView<ItemKeyboard>(std::string image);