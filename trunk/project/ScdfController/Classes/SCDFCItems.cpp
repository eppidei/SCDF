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
#include "LoadSavePanel.h"

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
        case ITEM_PAD_ID:
            item=ItemPad::create();
            break;
        case ITEM_KEYBOARD_ID:
            item=ItemKeyboard::create();
            break;
        case ITEM_MULTIPAD_ID:
            item=ItemMultipad::create();
            break;
        case ITEM_KNOB_ID:
            item=ItemKnob::create();
            break;
        case ITEM_SWITCH_ID:
            item=ItemSwitch::create();
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

ItemBase::ItemBase() : controlUnit(new ScdfCtrl::ControlUnit()), inflateVValue(0), inflateHValue(0), colorIndex(Colors::ItemsColorsId::Gray)
{
}

void ItemBase::CalculateItemSize(float gridBase)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.width+=inflateHValue;
    baseSize.height+=inflateVValue;
    setContentSize(cocos2d::Size(baseSize.width*gridBase, baseSize.height*gridBase));
}

void ItemBase::IncrementInflate(bool isHeight)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    float gridUnity=getContentSize().width/(baseSize.width+inflateHValue);
    
    cocos2d::Size parentSize=getParent()->getContentSize();
    if (baseSize.width>baseSize.height)
    {
        if (isHeight)
        {
            float diff=baseSize.width-baseSize.height;
            inflateVValue=fmin(inflateVValue+1,inflateHValue+diff-1);
        }
        else
            inflateHValue=fmin(inflateHValue+1,parentSize.width/gridUnity);
    }
    else if(baseSize.width<baseSize.height)
    {
        if (isHeight)
            inflateVValue=fmin(inflateVValue+1,parentSize.height/gridUnity);
        else
        {
            float diff=baseSize.height-baseSize.width;
            inflateHValue=fmin(inflateHValue+1,inflateVValue+diff-1);
        }
    }
    else
    {   float size=fmin(parentSize.height/gridUnity,parentSize.width/gridUnity);
        if (isHeight)
            inflateVValue=fmin(inflateVValue+1,size);
        else
            inflateHValue=fmin(inflateHValue+1,size);
    }
    
    CalculateItemSize(gridUnity);
}

void ItemBase::DecrementInflate(bool isHeight)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    float gridUnity=getContentSize().width/(baseSize.width+inflateHValue);
    if (baseSize.width>baseSize.height)
    {
        if (isHeight)
            inflateVValue=fmax(inflateVValue-1,1-baseSize.height);
        else
            inflateHValue=fmax(inflateHValue-1,1-baseSize.height);
    }
    else if (baseSize.width<baseSize.height)
    {
        if (isHeight)
            inflateVValue=fmax(inflateVValue-1,1-baseSize.width);
        else
            inflateHValue=fmax(inflateHValue-1,1-baseSize.width);
    }
    else
    {
        if (isHeight)
            inflateVValue=fmax(inflateVValue-1,-1);
        else
            inflateHValue=fmax(inflateHValue-1,-1);
    }
    CalculateItemSize(gridUnity);
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
    min=0;
    max=127;
    value=64;
    thumb = Layout::create();
    addChild(thumb,2);

    thumb->setTouchEnabled(true);
    thumb->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //thumb->setBackGroundColor(Color3B::RED);
    thumb->setAnchorPoint(Vec2(0.5,0.5));
    thumb->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    DoCreateThumb();
    SetThumbPosition();
}

cocos2d::Size ItemSlider::GetThumbSize(cocos2d::Size currentSize)
{
    float size=fmin(currentSize.height,currentSize.width);
    cocos2d::Size s(size+35, size+5);
    return s;
}
void ItemSlider::DoCreateThumb()
{
    thumb->setContentSize(GetThumbSize(getContentSize()));
    thumb->setBackGroundImage("sliderHAT.png");
    const int bitmapOffsetLeft=22;
    const int bitmapOffsetTop=13;
    const int bitmapOffsetRight=7;
    const int bitmapOffsetBottom=12;
    cocos2d::Rect r(bitmapOffsetLeft, bitmapOffsetTop, thumb->getBackGroundImageTextureSize().width-bitmapOffsetLeft-bitmapOffsetRight, thumb->getBackGroundImageTextureSize().height-bitmapOffsetTop-bitmapOffsetBottom);
    thumb->setBackGroundImageScale9Enabled(true);
    thumb->setBackGroundImageCapInsets(r);
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
    SetVertical(false);
    slideBar = Layout::create();
    addChild(slideBar);
    slideBar->setTouchEnabled(true);
    slideBar->setAnchorPoint(Vec2(0,1));
    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    //slideBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    //slideBar->setBackGroundColor(Color3B::BLACK);
    slideBar->setBackGroundImage("sliderBARStripe.png");
    const int bitmapcapInsetOffset=5;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, slideBar->getBackGroundImageTextureSize().width, slideBar->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(getContentSize().width/3, getContentSize().height);
    slideBar->setBackGroundImageScale9Enabled(true);
    slideBar->setBackGroundImageCapInsets(rr);
    slideBar->setContentSize(s);
    slideBar->setPosition(Vec2(getContentSize().width/2-slideBar->getContentSize().width/2, getContentSize().height));
    
    
    slideBarOff = Layout::create();
    addChild(slideBarOff);
    slideBarOff->setTouchEnabled(true);
    slideBarOff->setAnchorPoint(Vec2(0,1));
    slideBarOff->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    slideBarOff->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //slideBarOff->setBackGroundColor(Color3B::BLACK);
//    slideBarOff->setBackGroundImageScale9Enabled(true);
//    slideBarOff->ignoreContentAdaptWithSize(true);
    slideBarOff->setBackGroundImage("sliderBAR.png");
    slideBarOff->setBackGroundImageScale9Enabled(true);
    slideBarOff->setBackGroundImageCapInsets(rr);
    slideBarOff->setContentSize(cocos2d::Size(s.width, s.height+2));
    slideBarOff->setPosition(Vec2(getContentSize().width/2-slideBarOff->getContentSize().width/2, getContentSize().height+2));
    
    //InitSliderLayout();

    CreateThumb();
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

#define RangeRadStart 4.27
#define RangeRadEnd 5.4105206811824216884634413823147
#define PHI 3.1415926535897932384626433832795

void ItemKnob::AngularMode(Vec2 touchCoordWorld)
{

    static double AngleConstantOffset=PHI/2-RangeRadStart;
    
    Vec2 center(getContentSize().width/2, getContentSize().height/2);

    Vec2 touchPos=convertToNodeSpace(touchCoordWorld);
    short int x=touchPos.x;
    short int y=touchPos.y;
    
    int dx=x-center.x;
    int dy=-y+center.y;
    double angle=atan2(double(dy), double(dx));

    angle-=AngleConstantOffset;
    
    if(angle>3*PHI/2) angle-=2*PHI;
    if(angle<0&&angle<-PHI/2) angle+=2*PHI;
    
    angle=RangeRadStart-angle;
    double val=angle/RangeRadEnd;
    long temp=(max-min)-long(min+(max-min)*val);
    
    if(abs(temp-value)<((max-min)*2/3))
        SetValue(temp);
}

Vec2 ItemSlider::OnMove(Widget *widget)
{
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
    if (abs(gainDiff)>=1)
    {
        //printf("DIFF INT VALUE %d\n", gainDiff);
        value+=gainDiff;
        SetValue(value);
    }
    return touchPos;
}

Vec2 ItemKnob::OnMove(Widget *widget)
{
    Vec2 touchPos=widget->getTouchMovePosition();
    AngularMode(touchPos);
    //return ItemSlider::OnMove(widget) //Linear movement like slider
    return touchPos;
}

void ItemSlider::OnItemTouchMoved(Widget *widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;

    Vec2 touchPosNew=OnMove(widget);
    
    if (callback.get())
        callback->OnItemTouchMoved(value);
    else
        controlUnit->SendValue(value);
    
    dragPosUpdated=touchPosNew;

    NotifyEvent(SCDFC_EVENTS_Move_Item);
}

void ItemSlider::SetThumbPosition()
{
    if (NULL==thumb) return;
    float offset=isVertical?thumb->getContentSize().height:thumb->getContentSize().width;
//    if (IsKnob()) offset=0;
    float size=isVertical?getContentSize().height-offset:getContentSize().width-offset;
    float position=((((float)value-min)/(abs(max-min)))*(/*value-min*/size))+offset/2;
    if (isVertical)
    {
        const int bitmapThumbXOffset=-8;
        thumb->setPosition(Vec2(getContentSize().width/2.0+bitmapThumbXOffset,position));
//        slideBarOff->setPosition(Vec2(slideBarOff->getPositionX(),position+slideBarOff->getContentSize().height));
        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,getContentSize().height-position+2));
    }
    else
        thumb->setPosition(Vec2(position,getContentSize().height/2.0));
}

cocos2d::Size ItemSlider::GetStaticBaseSize()
{
    if (isVertical)
        return GetBaseSize();
    return SLIDER_HORZ_SIZE_BASE;
}

void ItemSlider::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    slideBar->setColor(Colors::Instance()->GetItemsColor(colorIndex));
}

void ItemSlider::setContentSize(const cocos2d::Size &contentSize)
{
    cocos2d::Size temp=contentSize;
    if (thumb)
        thumb->setContentSize(GetThumbSize(contentSize));
    if (!IsKnob())
        temp=GetSizeConsideringOrientation(contentSize);

    //Layout::setContentSize(temp);
    Node::setContentSize(temp);
    SetValue(value);
}

//void ItemSlider::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{return;
//    Color3B cc=Colors::Instance()->GetItemsColor(colorIndex);
//    Color3B main_cc=Colors::Instance()->GetUIColor(Colors::Main_Background);
//    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F(((float)main_cc.r)/255.f,((float)main_cc.g)/255.f,((float)main_cc.b)/255.f,1.0));
//    Color4F c(((float)cc.r)/255.f,((float)cc.g)/255.f,((float)cc.b)/255.f,1.0);
//    if (isVertical)
//        DrawPrimitives::drawSolidRect(Vec2(0, thumb->getPositionY()+thumb->getContentSize().width/2), Vec2(getContentSize().width, 0), c);
//    else
//        DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(thumb->getPositionX()+thumb->getContentSize().width/2, 0), c);
//    
////    DrawPrimitives::setDrawColor4B(50, 200, 50, 255);
////    DrawPrimitives::drawRoundedRect(getContentSize().width*0.8, getContentSize().height*0.8, 5, 10);
//}

void ItemKnob::UpdateOnPointVisibility()
{
    int selectedPoint=(16.0f/(abs(max-min)))*(value-min);
    for (int i=0;i<onPoints.size();++i)
        onPoints[i]->setVisible(false);
    onPoints[selectedPoint]->setVisible(true);
}

void ItemKnob::SetThumbPosition()
{
    if (NULL==thumb) return;
    thumb->setPosition(Vec2(getContentSize().width/2.0,getContentSize().height/2.0));
    float rotation=(270.0f/(abs(max-min)))*(value-min);
    thumb->setRotation(rotation);
    UpdateOnPointVisibility();
}

cocos2d::Size ItemKnob::GetThumbSize(cocos2d::Size currentSize)
{
    float size=fmin(currentSize.height,currentSize.width);
    cocos2d::Size s(size,size);
    return s;
}

void ItemKnob::CreateONPoints()
{
    for (int i=1;i<18;++i)
    {
        char str[256];
        sprintf(str, "knobVOL%d.png",i);
        auto point = Layout::create();
        addChild(point,1);
        point->setVisible(false);
        point->setAnchorPoint(Vec2(0,1));
        point->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
        point->setBackGroundImage(str);
        cocos2d::Rect r(0, 0, point->getBackGroundImageTextureSize().width, point->getBackGroundImageTextureSize().height);
        point->setBackGroundImageScale9Enabled(true);
        point->setBackGroundImageCapInsets(r);
        point->setContentSize(getContentSize());
        point->setPosition(Vec2(0, getContentSize().height));
        onPoints.push_back(point);
    }
}

void ItemKnob::DoCreateThumb()
{
    CreateONPoints();
    thumb->setBackGroundImage("knobVOLPOINT.png");
    const int bitmapOffsetLeft=0;
    const int bitmapOffsetTop=0;
    const int bitmapOffsetRight=0;
    const int bitmapOffsetBottom=0;
    cocos2d::Rect r(bitmapOffsetLeft, bitmapOffsetTop, thumb->getBackGroundImageTextureSize().width-bitmapOffsetLeft-bitmapOffsetRight, thumb->getBackGroundImageTextureSize().height-bitmapOffsetTop-bitmapOffsetBottom);
    thumb->setBackGroundImageScale9Enabled(true);
    thumb->setBackGroundImageCapInsets(r);
    thumb->setContentSize(GetThumbSize(getContentSize()));
    
    auto knob  = Layout::create();
    addChild(knob,1);
    //knob->setTouchEnabled(true);
    knob->setAnchorPoint(Vec2(0,1));
    knob->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    knob->setBackGroundImage("knobVOLKNOB.png");
    const int bitmapcapInsetOffset=0;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, knob->getBackGroundImageTextureSize().width, knob->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(getContentSize().width, getContentSize().height);
    knob->setBackGroundImageScale9Enabled(true);
    knob->setBackGroundImageCapInsets(rr);
    knob->setContentSize(s);
    knob->setPosition(Vec2(0, getContentSize().height));
    
    auto points = Layout::create();
    addChild(points);
    points->setAnchorPoint(Vec2(0,1));
    points->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    points->setBackGroundImage("knobVOL0.png");
    points->setBackGroundImageScale9Enabled(true);
    points->setBackGroundImageCapInsets(rr);
    points->setContentSize(s);
    points->setPosition(Vec2(0, getContentSize().height));
}

void ItemKnob::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    if (thumb)
        thumb->setColor(Colors::Instance()->GetItemsColor(colorIndex));
    for (int i=0;i<onPoints.size();++i)
        onPoints[i]->setColor(Colors::Instance()->GetItemsColor(colorIndex));
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

//void ItemKnob::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{return;
//    //Sprite::draw(renderer, transform, flags);return;
//    float degreesFromValue=(270.0/127.0)*value;
//    DrawPrimitives::setDrawColor4B(30, 30, 30, 255);
//    DrawPrimitives::drawSolidCircle(Vec2(getContentSize().width/2, getContentSize().height/2), getContentSize().width/2, CC_DEGREES_TO_RADIANS(180), 100);
//    DrawPrimitives::setDrawColor4B(200, 200, 200, 255);
//    GLfloat width;
//    glGetFloatv(GL_LINE_WIDTH,&width);
//    glLineWidth(2);
//    
//    GLfloat LineRange[2];
//    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,LineRange);
//    
//    Color3B cc=Colors::Instance()->GetItemsColor(colorIndex);
//    DrawPrimitives::drawCircle(Vec2(getContentSize().width/2, getContentSize().height/2), (getContentSize().width/2)-10, CC_DEGREES_TO_RADIANS((225-degreesFromValue)), 100,true);
//    DrawPrimitives::setDrawColor4B(cc.r,cc.g,cc.b,255);
//    
//    glLineWidth(10);
//
//#ifndef ANDROID
//    DrawPrimitives::DrawArc(getContentSize().width/2, getContentSize().height/2, getContentSize().width/2-5, CC_DEGREES_TO_RADIANS((225-degreesFromValue)), CC_DEGREES_TO_RADIANS(degreesFromValue), 50);
//    glLineWidth(width);
//#endif
//}

void ItemPad::Create()
{
    value=127;
    DEFAULT_NAME("Pad")
    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
//    setTexture->
    pad->loadTextures("padDefault.png", "padHover.png","padDefault.png");
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

void ItemPad::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    pad->setColor(Colors::Instance()->GetItemsColor(colorIndex));
    //    setColor(Colors::Instance()->GetItemsColor(colorIndex));
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

void ItemMultipad::Create()
{
    DEFAULT_NAME("Multipad")
    padSizeMultiply=1;
    padIndex=-1;
    CreatePads();
}

void ItemMultipad::ClearPads()
{
    for (int i=0;i<pads.size();++i)
        removeChild(pads[i],true);
    pads.clear();
}

void ItemMultipad::CreatePads()
{
    ClearPads();
    float totalPadsMultiply=MainScene::GetUnityBase()*padSizeMultiply;
    float padsWidth=ItemPad::GetBaseSize().width*totalPadsMultiply;
    float padsHeight=ItemPad::GetBaseSize().height*totalPadsMultiply;

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

void ItemMultipad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return;
    UpdateSelectedPadIndex(pad);
    ItemBase::OnItemTouchBegan(widget, type);
    pad->OnItemTouchBegan(widget, type);
}

void ItemMultipad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return;
    pad->OnItemTouchEnded(widget, type);
}

ItemMultipad::~ItemMultipad()
{
    ClearPads();
}

ItemPad *ItemMultipad::GetSelectedPad()
{
    if (-1==padIndex) return NULL;
    return pads[padIndex];
}

void ItemMultipad::UpdateSelectedPadIndex(ItemPad *pad)
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

void ItemKeyboard::Create()
{
    DEFAULT_NAME("Keyboard")
    currentOctave=4;
    selectedKey=-1;
    keysHandle = Layout::create();
    addChild(keysHandle);
    keysHandle->setTouchEnabled(true);
   // keysHandle->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    keysHandle->setAnchorPoint(Vec2(0,1));
    keysHandle->setContentSize(getContentSize());
    keysHandle->setPosition(Vec2(0,getContentSize().height));
    
    keysHandle->setBackGroundImage("pianoDefault.png");
    const int bitmapOffsetLeft=0;
    const int bitmapOffsetTop=0;
    const int bitmapOffsetRight=0;
    const int bitmapOffsetBottom=0;
    
    cocos2d::Rect r(bitmapOffsetLeft, bitmapOffsetTop, keysHandle->getBackGroundImageTextureSize().width-bitmapOffsetLeft-bitmapOffsetRight, keysHandle->getBackGroundImageTextureSize().height-bitmapOffsetTop-bitmapOffsetBottom);
    
    topBitmapOffsetPercentageForPressedKey=50.0/r.size.height;
    leftBitmapOffsetPercentageForPressedKey=22.0/r.size.width;
    
    keysHandle->setBackGroundImageScale9Enabled(true);
    keysHandle->setBackGroundImageCapInsets(r);
    keysHandle->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    
    blackKeyPressed=Sprite::create("pianoBlackPressed.png");
    whiteKeyPressed=Sprite::create("pianoWhitePressed.png");
    keysHandle->addChild(blackKeyPressed);
    blackKeyPressed->setAnchorPoint(Vec2(0,1));
    blackKeyPressed->setScale(getContentSize().width/r.size.width, getContentSize().height/r.size.height);
    blackKeyPressed->setVisible(false);
    keysHandle->addChild(whiteKeyPressed);
    whiteKeyPressed->setAnchorPoint(Vec2(0,1));
    whiteKeyPressed->setScale(getContentSize().width/r.size.width, getContentSize().height/r.size.height);
    whiteKeyPressed->setVisible(false);
}

const float numKeyboardKeys=12.0;

void ItemKeyboard::UpdateSelectedKey()
{
    const float velo=127.0;
    float keyWidth=(getContentSize().width-(2*leftBitmapOffsetPercentageForPressedKey*getContentSize().width))/numKeyboardKeys;
    Vec2 nodeCoord=keysHandle->convertToNodeSpace(dragPosUpdated);
    selectedKey=fmax(0,fmin(11,(nodeCoord.x-(leftBitmapOffsetPercentageForPressedKey*getContentSize().width))/keyWidth));
    int vel=velo*(getContentSize().height-nodeCoord.y)/getContentSize().height;
    SetValue(vel);
    controlUnit->SetMidiPitch(numKeyboardKeys*currentOctave+selectedKey);
    controlUnit->SendValue(vel);
    
    if (selectedKey==1||selectedKey==3||selectedKey==6||selectedKey==8||selectedKey==10)
    {
        blackKeyPressed->setPosition(Vec2(leftBitmapOffsetPercentageForPressedKey*getContentSize().width+keyWidth*selectedKey,getContentSize().height-(topBitmapOffsetPercentageForPressedKey*getContentSize().height)));
        blackKeyPressed->setVisible(true);
        whiteKeyPressed->setVisible(false);
    }
    else
    {
        whiteKeyPressed->setPosition(Vec2(leftBitmapOffsetPercentageForPressedKey*getContentSize().width+keyWidth*selectedKey,getContentSize().height-(topBitmapOffsetPercentageForPressedKey*getContentSize().height)));
        blackKeyPressed->setVisible(false);
        whiteKeyPressed->setVisible(true);
    }
    printf("Keyboard sends MIDI note %d with vel %d\n", controlUnit->GetMidiPitch(), vel);
}

void ItemKeyboard::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchBegan(widget, type);
    UpdateSelectedKey();
}

void ItemKeyboard::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchEnded(widget, type);
    selectedKey=-1;
    whiteKeyPressed->setVisible(false);
    blackKeyPressed->setVisible(false);
    controlUnit->SendValue(0);
}

void ItemKeyboard::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchMoved(widget, type);
    UpdateSelectedKey();
    NotifyEvent(ScdfCtrl::SCDFC_EVENTS_Move_Item);
}

//void ItemKeyboard::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{
//    Color3B cc=Colors::Instance()->GetItemsColor(colorIndex);
//    Color4F ccf=Color4F(cc.r/255.0,cc.g/255.0,cc.b/255.0,1.0);
//    Color4F selectedKeyColor=Color4F(20.0/255.0,20.0/255.0,1.0, 0.2);
//    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F::WHITE);
//    DrawPrimitives::setDrawColor4B(40, 40, 40, 255);
//    float keyWidth=getContentSize().width/numKeyboardKeys;
//    for (int i=0;i<numKeyboardKeys;++i)
//    {
//        Color4F currentColor=Color4F::WHITE;
//        if (i==1||i==3||i==6||i==8||i==10)
//            currentColor=ccf;
//        
//        DrawPrimitives::drawSolidRect(Vec2(i*keyWidth, getContentSize().height), Vec2(keyWidth*(i+1), 0), currentColor);
//        if (i==selectedKey)
//            DrawPrimitives::drawSolidRect(Vec2(i*keyWidth, getContentSize().height), Vec2(keyWidth*(i+1), 0), selectedKeyColor);
//        DrawPrimitives::drawRect(Vec2(i*keyWidth, getContentSize().height), Vec2(keyWidth*(i+1), 0));
//    }
//    DrawPrimitives::drawRect(Vec2(0, getContentSize().height), Vec2(getContentSize().height, 0));
//    DrawPrimitives::drawRect(Vec2(1, getContentSize().height-1), Vec2(getContentSize().width-1, 1));
//    //Layout::draw(renderer, transform, flags);
//}

ItemKeyboard::~ItemKeyboard()
{
    removeAllChildren();
}
