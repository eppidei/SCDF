//
//  SCDFCItems.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 18/11/14.
//
//

#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "ControlUnit.h"

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
            item=ItemKeyboard::create();
            break;
        case ITEM_KNOB_ID:
            item=ItemKnob::create();
            break;
        case ITEM_PAD_ID:
            item=ItemPad::create();
            break;
        case ITEM_SENSOR1_ID:
            item=ItemSensor1::create();
            break;
        default:
            return NULL;
    }
    item->setContentSize(r.size);
    item->setPosition(r.origin);
    item->Create();
    return item;
}

ItemBase::ItemBase() : controlUnit(new ScdfCtrl::ControlUnit())
{
}

void ItemBase::ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Widget* widget = dynamic_cast<Widget*>(pSender);
    WorkingPanel *wPanel=dynamic_cast<WorkingPanel*>(getParent());
    if (NULL==wPanel)
    {
        ItemBase *item=dynamic_cast<ItemBase*>(getParent());
        if (NULL!=item)
            item->ItemsTouchCallback(pSender,type);
    }
    else if (type==Widget::TouchEventType::BEGAN)
        Notify();
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (NULL==wPanel||!wPanel->OnControlMove(this, widget->getTouchBeganPosition(), type))
                OnItemTouchBegan(widget, type);
            break;
        case Widget::TouchEventType::MOVED:
            if (NULL==wPanel||!wPanel->OnControlMove(this, widget->getTouchMovePosition(), type))
                OnItemTouchMoved(widget, type);
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            if (NULL==wPanel||!wPanel->OnControlMove (this, widget->getTouchEndPosition(), type))
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
    thumb->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //thumb->setBackGroundColor(Color3B::RED);
    thumb->setAnchorPoint(Vec2(0.5,0.5));
    float size=fmin(getContentSize().height,getContentSize().width);
    thumb->setContentSize(Size(size,size));
    SetThumbPosition();
    thumb->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
}

ItemSlider::~ItemSlider()
{
    if (slideBar)
        removeChild(slideBar,true);
    removeChild(thumb,true);
}

void ItemSlider::Create()
{
    isVertical=true;
    slideBar = Layout::create();
    slideBar->setTouchEnabled(true);
    slideBar->setAnchorPoint(Vec2(0,1));
    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    //slideBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    //slideBar->setBackGroundColor(Color3B::GREEN);
    slideBar->setBackGroundImageScale9Enabled(true);
    slideBar->setBackGroundImage("SliderBack.png");
    slideBar->setContentSize(getContentSize());
    
   // Rect rc;
    //rc.origin = Vec2(0,20);
    //rc.size = Size(20,20);
    //slideBar->setBackGroundImageCapInsets(rc);
    //slideBar->setBackGroundImageScale9Enabled(true);
    
    
    addChild(slideBar);
    InitSliderLayout();
    CreateThumb();
    
    thumb->setBackGroundImageScale9Enabled(true);
    thumb->setBackGroundImage("SliderHandle.png");
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
        distanceX=abs(dragStartPos.x-touchPos.x);
        distanceY=abs(dragStartPos.y-touchPos.y);
    }
    
    //float touchDistanceFromStart=sqrt(pow(distanceX,2)+pow(distanceY,2));
    float touchDistanceFromStart=isVertical?distanceX:distanceY;//sqrt(pow(distanceX,2)+pow(distanceY,2));
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
    printf("KNOB VALUE %f\n", value);
    dragPosUpdated=touchPos;
    controlUnit->SendValue(value);
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

void ItemKnob::SetThumbPosition()
{
    thumb->setPosition(Vec2(getContentSize().width/2,getContentSize().height/2));
    float rotation=(270.0f/(abs(max-min)))*value;
    thumb->setRotation(rotation);
}


void ItemKnob::Create()
{
    slideBar=NULL;
    isVertical=true;
    ItemSlider::CreateThumb();
   // thumb->setBackGroundImage("test_knob.png");
}

void ItemKnob::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    float degreesFromValue=(270.0/127.0)*value;
    DrawPrimitives::setDrawColor4B(30, 30, 30, 255);
    DrawPrimitives::drawSolidCircle(Vec2(getContentSize().width/2, getContentSize().height/2), getContentSize().width/2, CC_DEGREES_TO_RADIANS(180), 100);
    DrawPrimitives::setDrawColor4B(200, 200, 200, 255);
    GLfloat width;
    glGetFloatv(GL_LINE_WIDTH,&width);
    glLineWidth(2);
    
    GLfloat LineRange[2];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,LineRange);
    
    DrawPrimitives::drawCircle(Vec2(getContentSize().width/2, getContentSize().height/2), (getContentSize().width/2)-10, CC_DEGREES_TO_RADIANS((225-degreesFromValue)), 100,true);
    DrawPrimitives::setDrawColor4B(127, 200, 128, 255);
    
    glLineWidth(10);
    DrawPrimitives::DrawArc(getContentSize().width/2, getContentSize().height/2, getContentSize().width/2-5, CC_DEGREES_TO_RADIANS((225-degreesFromValue)), CC_DEGREES_TO_RADIANS(degreesFromValue), 50);
    glLineWidth(width);
}

void ItemPad::Create()
{
    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
    pad->loadTextures("CloseNormal.png", "CloseSelected.png","CloseNormal.png");
    pad->setAnchorPoint(Vec2(0,1));
    pad->setPosition(Vec2(0,getContentSize().height));
    pad->setContentSize(getContentSize());
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    
    Rect rc;
    rc.origin = Vec2(16,16);
    rc.size = Size(114,114);
    pad->setCapInsets(rc);
    addChild(pad);
}

void ItemPad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=(ItemPad*)widget;
    controlUnit->SendValue(127);
}

void ItemPad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=(ItemPad*)widget;
    controlUnit->SendValue(0);
}

ItemPad::~ItemPad()
{
    pad->addTouchEventListener(nullptr);
    removeChild(pad);
}

void ItemKeyboard::Create()
{
    padSizeMultiply=1;
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //setBackGroundColor(Color3B::BLUE);
    CreatePads();
}

void ItemKeyboard::ClearPads()
{
    for (int i=0;i<pads.size();++i)
        removeChild(pads[i]);
    pads.clear();
}

void ItemKeyboard::CreatePads()
{
    ClearPads();
    float totalPadsMultiply=MainScene::GetUnityBase()*padSizeMultiply;
    float padsWidth=ItemPad::GetSize().width*totalPadsMultiply;
    float padsHeight=ItemPad::GetSize().height*totalPadsMultiply;

    int numPadsRow=getContentSize().width/padsWidth;
    int numPadsColumn=getContentSize().height/padsHeight;
    for (int column=0;column<numPadsColumn;++column)
    {
        for (int row=0;row<numPadsRow;++row)
        {
            Rect r(row*padsWidth,getContentSize().height-column*padsHeight,padsWidth,padsHeight);
            ItemPad *pad=(ItemPad*)ItemBase::CreateItem(r, ITEM_PAD_ID);
            pad->setAnchorPoint(Vec2(0,1));
            addChild(pad);
        }
    }
    
}

ItemKeyboard::~ItemKeyboard()
{
    ClearPads();
}