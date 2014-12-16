//
//  SCDFCItems.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 18/11/14.
//
//

#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
using namespace SCDFC;
USING_NS_CC;
using namespace ui;

ItemBase *ItemBase::CreateItem(Rect r,  int itemID)
{
    ItemBase *item=NULL;
    switch (itemID)
    {
        case ITEM_SLIDER_ID:
            item=ItemSlider::create();
            break;
        case ITEM_KEYBOARD_ID:
            item=Keyboard::create();
            break;
        case ITEM_KNOB_ID:
            item=Knob::create();
            break;
        case ITEM_PAD_ID:
            item=ItemPad::create();
            break;
        case ITEM_SENSOR1_ID:
            item=Sensor1::create();
            break;
        default:
            return NULL;
    }
    item->setContentSize(r.size);
    item->setPosition(r.origin);
    item->Create();
    return item;
}

void ItemBase::ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Widget* widget = dynamic_cast<Widget*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (!((WorkingPanel*)getWidgetParent())->OnControlMove(this, widget->getTouchBeganPosition(), type))
                OnItemTouchBegan(widget, type);
            break;
        case Widget::TouchEventType::MOVED:
            if (!((WorkingPanel*)getWidgetParent())->OnControlMove(this, widget->getTouchMovePosition(), type))
                OnItemTouchMoved(widget, type);
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            if (!((WorkingPanel*)getWidgetParent())->OnControlMove (this, widget->getTouchEndPosition(), type))
                OnItemTouchEnded(widget, type);
            break;
        default:
            break;
    }
}

void ItemBase::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
}
void ItemBase::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    dragPosUpdated=widget->getTouchMovePosition();
}

void ItemSlider::InitSliderLayout()
{
    bool nowIsVertical=getContentSize().height>getContentSize().width;

    if ((nowIsVertical&&!isVertical) || (!nowIsVertical&&!isVertical))
        setContentSize(Size(getContentSize().height,getContentSize().width));
    
    if (isVertical)
    {
        slideBar->setContentSize(Size(getContentSize().width/SLIDER_SIZE_BASE.width, getContentSize().height));
        slideBar->setPosition(Vec2(getContentSize().width/2.0-slideBar->getContentSize().width/2.0,getContentSize().height));
    }
    else
    {
        slideBar->setContentSize(Size(getContentSize().width, getContentSize().height/SLIDER_SIZE_BASE.width));
        slideBar->setPosition(Vec2(0,getContentSize().height/2.0+slideBar->getContentSize().height/2.0));
    }
}

void ItemSlider::CreateThumb()
{
    min=0;
    max=127;
    value=64;
    thumb = Layout::create();
    addChild(thumb);
    thumb->setTouchEnabled(true);
//    thumb->setBackGroundImageScale9Enabled(true);
//    thumb->setBackGroundImage("CloseNormal.png");
    thumb->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    thumb->setBackGroundColor(Color3B::RED);
    thumb->setAnchorPoint(Vec2(0.5,0.5));
    float size=fmin(getContentSize().height,getContentSize().width);
    thumb->setContentSize(Size(size,size));
    SetThumbPosition();
    thumb->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
}

ItemSlider::~ItemSlider()
{
    if (slideBar)
        removeChild(slideBar);
    removeChild(thumb);
}

void ItemSlider::Create()
{
    isVertical=true;
    slideBar = Layout::create();
    slideBar->setTouchEnabled(true);
    slideBar->setAnchorPoint(Vec2(0,1));
    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    slideBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    slideBar->setBackGroundColor(Color3B::GREEN);
    addChild(slideBar);
    InitSliderLayout();
    CreateThumb();
}

void ItemSlider::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    ItemBase::OnItemTouchBegan(widget, type);
}

void ItemSlider::OnItemTouchMoved(Widget *widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    Vec2 touchPos=widget->getTouchMovePosition();
    float distanceX=0, distanceY=0;
    Rect rItem(getPositionX(), getPositionY()-getContentSize().height,getContentSize().width, getContentSize().height);
    if (!rItem.containsPoint(touchPos))
    {
        distanceX=dragStartPos.x-touchPos.x;
        distanceY=dragStartPos.y-touchPos.y;
    }
    
    float touchDistanceFromStart=sqrt(pow(distanceX,2)+pow(distanceY,2));
    if (isVertical){
        float diff=touchPos.y-dragPosUpdated.y;
        float draggingVel=log(1+touchDistanceFromStart)+1;
        if (IsKnob())
            draggingVel=2*log(1+touchDistanceFromStart)+1;
        diff/=draggingVel;
        value+=(diff*(abs(max-min))/getContentSize().height);
    }
    else{
        float diff=touchPos.x-dragPosUpdated.x;
        float draggingVel=log(1+touchDistanceFromStart)+1;
        if (IsKnob())
            draggingVel=2*log(1+touchDistanceFromStart)+1;
        diff/=draggingVel;
        value+=(diff*(abs(max-min))/getContentSize().width);
    }
    value=fmax(min,fmin(max,value));
    dragPosUpdated=touchPos;
    SetThumbPosition();
}

void ItemSlider::SetThumbPosition()
{
    float size=isVertical?getContentSize().height:getContentSize().width;
    float position=(size/(abs(max-min)))*value;
    if (isVertical)
        thumb->setPosition(Vec2(getContentSize().width/2.0,position));
    else
        thumb->setPosition(Vec2(position,getContentSize().height/2.0));
}

void Knob::SetThumbPosition()
{
    float rotation=(270.0f/(abs(max-min)))*value;
    thumb->setRotation(rotation);
}

void Knob::Create()
{
    slideBar=NULL;
    isVertical=false;
    ItemSlider::CreateThumb();
    thumb->setBackGroundImage("2_knob_anim0015.png");
}

void ItemPad::Create()
{
    pad = Button::create();
    pad->setScale9Enabled(true);
    pad->loadTextures("CloseNormal.png", "CloseSelected.png");
    pad->setTouchEnabled(true);
    pad->setAnchorPoint(Vec2(0,1));
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    addChild(pad);
}

ItemPad::~ItemPad()
{
    removeChild(pad);
}