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

using namespace ScdfCtrl;
USING_NS_CC;
using namespace ui;

#define DEFAULT_NAME(x) \
        static int counter=0; \
        std::ostringstream os; \
        os<<x<<counter; \
        name=os.str(); \
        counter=++counter%50; \

ItemBase *ItemBase::CreateItem(cocos2d::Rect r,  int itemID)
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
    item->setAnchorPoint(Vec2(0,1));
    item->setContentSize(r.size);
    item->setPosition(r.origin);
    item->Create();
    return item;
}

ItemBase::ItemBase() : controlUnit(new ScdfCtrl::ControlUnit()), sizeMultiply(1)
{
}

void ItemBase::SetItemMultiply(int multiply)
{
    cocos2d::Size s=getContentSize();
    cocos2d::Size baseSize=cocos2d::Size(getContentSize().width/sizeMultiply, getContentSize().height/sizeMultiply);
    cocos2d::Size newSize=cocos2d::Size(baseSize.width*multiply, baseSize.height*multiply);
    sizeMultiply=multiply;
    setContentSize(newSize);
}

void ItemBase::ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Widget* widget = dynamic_cast<Widget*>(pSender);
    WorkingPanel *wPanel=dynamic_cast<WorkingPanel*>(getParent());
    if (NULL==wPanel)
    {
        ItemBase *item=dynamic_cast<ItemBase*>(getParent());
        if (NULL!=item)
        {
            item->ItemsTouchCallback(pSender,type);
            return;
        }
    }
    
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
    NotifyEvent(SCDFC_EVENTS_Select_Item);
    dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
}
void ItemBase::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    dragPosUpdated=widget->getTouchMovePosition();
}

cocos2d::Size ItemSlider::GetSizeConsideringOrientation(const cocos2d::Size resized)
{
    cocos2d::Size ret;
    if (isVertical)
        ret=cocos2d::Size(fmin(resized.height, resized.width),fmax(resized.height, resized.width));
    else
        ret=cocos2d::Size(fmax(resized.height, resized.width),fmin(resized.height, resized.width));
    return ret;
}

void ItemSlider::CreateThumb()
{
    thumb=NULL;
//    return;
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
    thumb->setContentSize(cocos2d::Size(size,size));
    SetThumbPosition();
    thumb->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
}

ItemSlider::~ItemSlider()
{
    if (slideBar)
        removeChild(slideBar,true);
    if (thumb)
        removeChild(thumb,true);
}

void ItemSlider::SetVertical(bool vertical)
{
    isVertical=vertical;
    setContentSize(getContentSize());
//    InitSliderLayout();
}

void ItemSlider::SetValue(float _value)
{
    value=fmax(min,fmin(max,_value));
    //#ifdef DEBUG
    printf("KNOB VALUE %d\n", value);
    //#endif
    SetThumbPosition();
}

void ItemSlider::SetRange(float _min, float _max)
{
    min=_min;
    max=_max;
    SetValue(value);
}

void ItemSlider::Create()
{
    DEFAULT_NAME("Slider")
//    slideBar = Layout::create();
//    slideBar->setTouchEnabled(true);
//    slideBar->setAnchorPoint(Vec2(0,1));
//    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
//    slideBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
//    slideBar->setBackGroundColor(Color3B::BLACK);
   // slideBar->setBackGroundImageScale9Enabled(true);
   // slideBar->setBackGroundImage("SliderBack.png");
    //slideBar->setContentSize(getContentSize());
    
   // Rect rc;
    //rc.origin = Vec2(0,20);
    //rc.size = Size(20,20);
    //slideBar->setBackGroundImageCapInsets(rc);
    //slideBar->setBackGroundImageScale9Enabled(true);
    
    
   // addChild(slideBar);
    //setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::NONE);
    //InitSliderLayout();
    SetVertical(true);
    CreateThumb();
    if (!thumb) return;
    thumb->setBackGroundImageScale9Enabled(true);
    thumb->setBackGroundImage("SliderHandle.png");
}

void ItemSlider::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    ItemBase::OnItemTouchBegan(widget, type);
    dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
    //dragStartPos=dragPosUpdated=thumb->getWorldPosition();
    if (callback.get())
        callback->OnItemTouchBegan();
}

void ItemSlider::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    ItemBase::OnItemTouchEnded(widget, type);
    if (callback.get())
        callback->OnItemTouchEnded();
}

void ItemSlider::OnItemTouchMoved(Widget *widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    Vec2 limits=convertToWorldSpace(Vec2(_anchorPoint.x * _contentSize.width, _anchorPoint.y * _contentSize.height));//getWorldPosition();
    Vec2 touchPos=widget->getTouchMovePosition();
    
    if (!IsKnob())
    {
        if (isVertical)
            touchPos.y=fmin(fmax(touchPos.y,limits.y-getContentSize().height),limits.y);
        else
            touchPos.x=fmin(fmax(touchPos.x,limits.x),limits.x+getContentSize().width);
    }
    
    float distanceX=0, distanceY=0;
    
    cocos2d::Rect rItem(getPositionX(), getPositionY()-getContentSize().height,getContentSize().width, getContentSize().height);

    if (!rItem.containsPoint(touchPos))
    {
        distanceX=abs(dragStartPos.x-touchPos.x);
        distanceY=abs(dragStartPos.y-touchPos.y);
    }
    
    int gainDiff=0;
    float touchDistanceFromStart=isVertical?distanceX:distanceY;//sqrt(pow(distanceX,2)+pow(distanceY,2));
    if (isVertical){
        float diff=touchPos.y-dragPosUpdated.y;
        float draggingVel=log(1+touchDistanceFromStart)+1;
        float sliderComp=thumb->getContentSize().height;
        if (IsKnob())
        {
            draggingVel=3*log(1+touchDistanceFromStart)+1;
            diff/=draggingVel;
            sliderComp=0;
        }
        gainDiff=(int)(diff*(abs(max-min))/(getContentSize().height-sliderComp));
    }
    else{
        float diff=touchPos.x-dragPosUpdated.x;
        float draggingVel=log(1+touchDistanceFromStart)+1;
        float sliderComp=thumb->getContentSize().width;
        if (IsKnob())
        {
            draggingVel=3*log(1+touchDistanceFromStart)+1;
            diff/=draggingVel;
            sliderComp=0;
        }
        gainDiff=(int)(diff*(abs(max-min))/(getContentSize().width-sliderComp));
    }
    if (abs(gainDiff)<1) return;
//#ifdef DEBUG
    printf("DIFF INT VALUE %d\n", gainDiff);
//#endif
    value+=gainDiff;
    SetValue(value);

    if (callback.get())
        callback->OnItemTouchMoved(value);
    else
        controlUnit->SendValue(value);
   // if (IsKnob())
        dragPosUpdated=touchPos;
    //else
      //  dragPosUpdated=thumb->getWorldPosition();
    NotifyEvent(SCDFC_EVENTS_Move_Item);
}

void ItemSlider::SetThumbPosition()
{
    if (NULL==thumb) return;
    float offset=isVertical?thumb->getContentSize().height:thumb->getContentSize().width;
    if (IsKnob()) offset=0;
    float size=isVertical?getContentSize().height-offset:getContentSize().width-offset;
    float position=((((float)value-min)/(abs(max-min)))*(/*value-min*/size))+offset/2;
    if (isVertical)
        thumb->setPosition(Vec2(getContentSize().width/2.0,position));
    else
        thumb->setPosition(Vec2(position,getContentSize().height/2.0));
}

void ItemSlider::setContentSize(const cocos2d::Size &contentSize)
{
    float size=fmin(contentSize.height,contentSize.width);
    cocos2d::Size temp=contentSize;
    if (thumb)
        thumb->setContentSize(cocos2d::Size(size,size));
    if (!IsKnob())
        temp=GetSizeConsideringOrientation(contentSize);

    Node::setContentSize(temp);
    SetValue(value);
}

void ItemSlider::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F(((float)MAIN_BACK_COLOR.r)/255.f,((float)MAIN_BACK_COLOR.g)/255.f,((float)MAIN_BACK_COLOR.b)/255.f,1.0));
    Color4F c(((float)color.r)/255.f,((float)color.g)/255.f,((float)color.b)/255.f,1.0);
    if (isVertical)
        DrawPrimitives::drawSolidRect(Vec2(0, thumb->getPositionY()+thumb->getContentSize().width/2), Vec2(getContentSize().width, 0), c);
    else
        DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(thumb->getPositionX()+thumb->getContentSize().width/2, 0), c);
    
//    DrawPrimitives::setDrawColor4B(50, 200, 50, 255);
//    DrawPrimitives::drawRoundedRect(getContentSize().width*0.8, getContentSize().height*0.8, 5, 10);
    //Layout::visit(renderer, transform, flags);
}

void ItemKnob::SetThumbPosition()
{
    if (NULL==thumb) return;
    thumb->setPosition(Vec2(getContentSize().width/2.0,getContentSize().height/2.0));
//    float rotation=(270.0f/(abs(max-min)))*(value-min);
//    thumb->setRotation(rotation);
}

void ItemKnob::SetColor(cocos2d::Color3B _color)
{
    ItemBase::SetColor(_color);
    setColor(_color);
}

void ItemKnob::Create()
{
    DEFAULT_NAME("Knob")
    slideBar=NULL;
    thumb=NULL;
    isVertical=true;
    ItemSlider::CreateThumb();
    
//    auto sprite=Sprite::create("test_knob.png");
//    sprite->setAnchorPoint(Vec2(0.5,0.5));
//    sprite->setPosition(Vec2(GetSize().width/2 ,-10));
//    addChild(sprite);
//    sprite=Sprite::create("test_knob.png");
//    sprite->setAnchorPoint(Vec2(0.5,0.5));
//    sprite->setPosition(Vec2(GetSize().width/2 ,GetSize().height+10));
//    addChild(sprite);
//    initWithFile();
   // thumb->setBackGroundImage("test_knob.png");
}

void ItemKnob::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    //Sprite::draw(renderer, transform, flags);return;
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
    DrawPrimitives::setDrawColor4B(color.r,color.g,color.b,255);
    
    glLineWidth(10);

#ifndef ANDROID
    DrawPrimitives::DrawArc(getContentSize().width/2, getContentSize().height/2, getContentSize().width/2-5, CC_DEGREES_TO_RADIANS((225-degreesFromValue)), CC_DEGREES_TO_RADIANS(degreesFromValue), 50);
    glLineWidth(width);
#endif
}

void ItemPad::Create()
{
    value=127;
    DEFAULT_NAME("Pad")
    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
    pad->loadTextures("CloseNormal.png", "CloseSelected.png","CloseNormal.png");
    pad->setAnchorPoint(Vec2(0,1));
    pad->setPosition(Vec2(0,getContentSize().height));
    pad->setContentSize(getContentSize());
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    pad->setOpacity(230);
    cocos2d::Rect rc;
    rc.origin = Vec2(16,16);
    rc.size = cocos2d::Size(114,114);
    pad->setCapInsets(rc);
    addChild(pad);
}

void ItemPad::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(getContentSize().width*0.05, getContentSize().height*0.95), Vec2(getContentSize().width*0.95, getContentSize().height*0.05), Color4F(color.r/255,color.g/255,color.b/255,255));
}

void ItemPad::setContentSize(const cocos2d::Size &contentSize)
{
    Node::setContentSize(contentSize);
    if (NULL==pad) return;
    pad->setContentSize(contentSize);
    pad->setPosition(Vec2(0,contentSize.height));
}

void ItemPad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchBegan(widget,type);
    printf("Pad %x sends noteOn\n", widget);
    controlUnit->SendValue(value);
}

void ItemPad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    printf("Pad %x sends noteOff\n", widget);
    controlUnit->SendValue(0);
}

ItemPad::~ItemPad()
{
    pad->addTouchEventListener(nullptr);
    removeChild(pad,true);
}

void ItemKeyboard::Create()
{
    DEFAULT_NAME("Keyboard")
    padSizeMultiply=1;
    padIndex=-1;
  //  setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //setBackGroundColor(Color3B::BLUE);
    CreatePads();
}

void ItemKeyboard::ClearPads()
{
    for (int i=0;i<pads.size();++i)
        removeChild(pads[i],true);
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
            padIndex=0;
            cocos2d::Rect r(row*padsWidth,getContentSize().height-column*padsHeight,padsWidth,padsHeight);
            ItemPad *pad=(ItemPad*)ItemBase::CreateItem(r, ITEM_PAD_ID);
            pad->setAnchorPoint(Vec2(0,1));
            pads.push_back(pad);
            addChild(pad);
        }
    }
    
}

void ItemKeyboard::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return;
    UpdateSelectedPadIndex(pad);
    ItemBase::OnItemTouchBegan(widget, type);
    pad->OnItemTouchBegan(widget, type);
}

void ItemKeyboard::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return;
    pad->OnItemTouchEnded(widget, type);
}

ItemKeyboard::~ItemKeyboard()
{
    ClearPads();
}

ItemPad *ItemKeyboard::GetSelectedPad()
{
    if (-1==padIndex) return NULL;
    return pads[padIndex];
}

void ItemKeyboard::UpdateSelectedPadIndex(ItemPad *pad)
{
    for (int i=0;i<pads.size();++i)
    {
        if (pads[i]==pad)
        {
            padIndex=i;
            return;
        }
    }
    padIndex=-1;
}
