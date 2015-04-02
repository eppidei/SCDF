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
//#include "MultiSender.h"
#include "LoadSavePanel.h"
#include "ControlUnit.h"
#include "MultiSender.h"

using namespace ScdfCtrl;
USING_NS_CC;
using namespace ui;

#define DEFAULT_NAME(x) \
        static int counter=0; \
        std::ostringstream os; \
        os<<x<<counter; \
        SetName(os.str()); \
        counter=++counter%50; \

void ItemBase::SetName(std::string _name)
{
	name = _name;
    if (label)
        label->SetText(name);
}

std::string ItemBase::GetName()
{
	return name;
}

int ItemBase::GetGroupID()
{
    return groupId;
}

Colors::ItemsColorsId ItemBase::GetColor()
{
	return colorIndex;
}

bool ItemBase::IsMaster()
{
    return isMaster;
}

void ItemBase::SetColor(Colors::ItemsColorsId _colorIndex)
{
	colorIndex = _colorIndex;
}

void ItemBase::SetGroupID(int _groupId)
{
    groupId = _groupId;
}

void ItemBase::SetMaster(bool _isMaster)
{
    isMaster = _isMaster;
}

cocos2d::Size ItemBase::GetControlContentSize()
{
    return control->getContentSize();
}

#define LABEL_X_OFFSET 5.0

void ItemBase::CreateItemBaseElements()
{
    cocos2d::Rect lRect(0,getContentSize().height,getContentSize().width,ITEMS_LABEL_HEIGHT);
    label=TextWithBackground::CreateText(-1,lRect, name,"arial",16);
    label->setColor(Color3B::BLACK);
//    label->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
   // label->SetAlignement();
    addChild(label,1);
    lRect.size.width=ITEMS_LABEL_HEIGHT-LABEL_X_OFFSET;
    lRect.size.height=ITEMS_LABEL_HEIGHT-LABEL_X_OFFSET;
    label1=TextWithBackground::CreateText(-1,lRect, name,"arial",16);
    label1->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    label1->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    //    label->SetAlignement(cocos2d::TextHAlignment::LEFT);
    addChild(label1,1);
    
    control=Layout::create();
    addChild(control,1);
    control->setAnchorPoint(Vec2(0,1));
    control->setContentSize(getContentSize());
    control->setPosition(Vec2(0,getContentSize().height));
}

void ItemBase::PlaceItemBaseElements()
{
    if (GetLayoutManager()->ControlIconOnTop(getContentSize()))
    {
        label1->setPosition(Vec2(getContentSize().width/2.0-label1->getContentSize().width/2.0,getContentSize().height));
        control->setPosition(Vec2(0,getContentSize().height-ITEMS_LABEL_HEIGHT));
        label->setPosition(Vec2(getContentSize().width/2.0-label->getContentSize().width/2.0,getContentSize().height-ITEMS_LABEL_HEIGHT-control->getContentSize().height-LABEL_X_OFFSET));
    }
    else
    {
        float itemInfoWidth=label1->getContentSize().width+label->getContentSize().width;
        control->setPosition(Vec2(0,getContentSize().height));
        label1->setPosition(Vec2(getContentSize().width/2.0-itemInfoWidth/2.0,getContentSize().height-control->getContentSize().height-LABEL_X_OFFSET));
        label->setPosition(Vec2(getContentSize().width/2.0-itemInfoWidth/2.0+label1->getContentSize().width,getContentSize().height-control->getContentSize().height-LABEL_X_OFFSET));
    }
}

void ItemBase::setContentSize(const cocos2d::Size &contentSize)
{
    Layout::setContentSize(cocos2d::Size(contentSize.width, contentSize.height));
    if (!control) return;
    float itemHeight=GetLayoutManager()->ControlIconOnTop(contentSize)?contentSize.height-2*ITEMS_LABEL_HEIGHT:contentSize.height-ITEMS_LABEL_HEIGHT;
    control->setContentSize(cocos2d::Size(contentSize.width, itemHeight));
    float labelWidth=GetLayoutManager()->ControlIconOnTop(contentSize)?contentSize.width:contentSize.width-ITEMS_LABEL_HEIGHT;
    label->setContentSize(cocos2d::Size(labelWidth, ITEMS_LABEL_HEIGHT-LABEL_X_OFFSET));

    PlaceItemBaseElements();
    DoSetContentSize(control->getContentSize());
}

template <class ItemType> ItemBase *ItemBase::CreateItem()
{
    ItemBase *item=ItemType::create();

    item->setAnchorPoint(Vec2(0,1));
    item->CreateItemBaseElements();
    item->GetLayoutManager()->SetMagValue(0);
    item->Create();
    item->SetColor(Colors::ItemsColorsId::Orange);
    return item;
}

ItemBase::ItemBase() : control(NULL), label(NULL), controlImage(NULL), groupId(-1), isMaster(false)
{
	ChangeControlUnit(ControlUnit::Wire);
    layoutManager.reset(new ItemLayoutManager(this));
}

ItemBase::~ItemBase()
{
    control->removeAllChildren();
    removeAllChildren();
}

void ItemBase::ChangeControlUnit(ControlUnit::Type t)
{
	controlUnit.reset(ControlUnit::Create(t));
	//controlUnit->SetItem(this);
}

cocos2d::Size ItemSlider::CalculateNewItemBaseSize(int magValue)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.height+=magValue;
    baseSize.width=(int)((baseSize.width*baseSize.height)/GetStaticBaseSize().height);
    return baseSize;
}

cocos2d::Size ItemLayoutManager::GetSizeConsideringOrientation(const cocos2d::Size resized)
{
    cocos2d::Size ret;
    if (IsVertical())
        ret=cocos2d::Size(fmin(resized.height, resized.width),fmax(resized.height, resized.width));
    else
        ret=cocos2d::Size(fmax(resized.height, resized.width),fmin(resized.height, resized.width));
    return ret;
}

bool ItemLayoutManager::ControlIconOnTop(cocos2d::Size itemBaseSize)
{
    return itemBaseSize.width<(3*ITEMS_LABEL_HEIGHT);
}

bool ItemLayoutManager::CheckNewSizeCollision(cocos2d::Rect r)
{
    WorkingPanel *w=dynamic_cast<WorkingPanel*>(item->getParent());
    if (!w) return false;
    
    bool collision;
    w->DoDetectCollisions(item, r, &collision);
    if (collision)
    {
        item->LaunchCollisionAnimation();
        return true;
    }
    return false;
}

void ItemBase::LaunchCollisionAnimation()
{
    for (auto it=getChildren().begin();it!=getChildren().end();++it)
    {
        if (dynamic_cast<TextWithBackground*>(*it) != NULL) continue;
        auto tintTo = TintTo::create(0.05f, 180.0f, 20.0f, 20.0f);
        auto callback = CallFunc::create([this](){
            
            for (auto itt=getChildren().begin();itt!=getChildren().end();++itt)
            {
                if (dynamic_cast<TextWithBackground*>(*itt) != NULL) continue;
                auto retintTo = TintTo::create(0.2f, 255.0f, 255.0f, 255.0f);
                (*itt)->runAction(retintTo);
            }
        });
        auto seq = Sequence::create(tintTo, callback, NULL);
        (*it)->runAction(seq);
    }
}

void ItemLayoutManager::SetVertical(bool vertical)
{
    if (isVertical==vertical) return;
    isVertical = vertical;
    cocos2d::Size newSize(CalculateNewSize(magValue));
    Vec2 center(item->getPositionX()+item->getContentSize().width/2.0, item->getPositionY()-item->getContentSize().height/2.0);
    cocos2d::Rect r(center.x-newSize.width/2.0, center.y+newSize.height/2.0, newSize.width, newSize.height);
    
    if (CheckNewSizeCollision(r))
    {
        isVertical=!isVertical;
        return;
    }
    item->setContentSize(newSize);
    item->InitLayoutOrientation(center);
}

cocos2d::Size ItemLayoutManager::CalculateNewSize(int _magValue)
{
    cocos2d::Size newSize=item->CalculateNewItemBaseSize(_magValue);
    newSize=cocos2d::Size(newSize.width*MainScene::GetGridDistance(), newSize.height*MainScene::GetGridDistance());
    newSize=GetSizeConsideringOrientation(newSize);
    newSize.height+=ITEMS_LABEL_HEIGHT;
    if (ControlIconOnTop(newSize))
        newSize.height+=ITEMS_LABEL_HEIGHT;
    return newSize;
    
}

void ItemLayoutManager::ZoomPlus()
{
    int magValueTemp=fmin(8,magValue+1);
    if (magValueTemp==magValue) return;

    cocos2d::Size newSize=CalculateNewSize(magValueTemp);
    cocos2d::Rect r(item->getPositionX(), item->getPositionY(), newSize.width, newSize.height);
    if (!CheckNewSizeCollision(r))
        SetMagValue(magValueTemp);
}

void ItemLayoutManager::ZoomMinus()
{
    int magValueTemp=fmax(0,magValue-1);
    if (magValueTemp==magValue) return;
    SetMagValue(magValueTemp);
}

void ItemLayoutManager::SetMagValue(int _magValue)
{
    magValue=_magValue;
    item->setContentSize(CalculateNewSize(magValue));
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
                wPanel->OnItemTouchBegan(this, widget, type);
            break;
        case Widget::TouchEventType::MOVED:
            if (NULL==wPanel||!wPanel->OnControlMove(this, widget->getTouchMovePosition(), type))
                wPanel->OnItemTouchMoved(this, widget, type);
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            if (NULL==wPanel||!wPanel->OnControlMove (this, widget->getTouchEndPosition(), type))
                wPanel->OnItemTouchEnded(this, widget, type);
            break;
        default:
            break;
    }
}

void ItemBase::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget->getParent()==control)
        NotifyEvent(SCDFC_EVENTS_Select_Item);
    dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
}
void ItemBase::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    dragPosUpdated=widget->getTouchMovePosition();
}

void ItemSlider::CreateThumb()
{
    thumb=NULL;
    GetControlUnit()->SetMin(0);
    GetControlUnit()->SetMax(127);
    GetControlUnit()->OnTouch(ControlUnit::TouchCode,0.5);

    thumb = Layout::create();
    control->addChild(thumb,2);

    thumb->setTouchEnabled(true);
    thumb->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    thumb->setAnchorPoint(Vec2(0.5,0.5));
    thumb->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    DoCreateThumb();
    SetPositionOfValueDependentComponent();
}

cocos2d::Size ItemSlider::GetThumbSize(cocos2d::Size currentSize)
{
    float size=fmin(currentSize.height,currentSize.width);
//    float size=fmin(GetBaseSize().height,GetBaseSize().width);
    cocos2d::Size s(size+35, size+5);
    return s;
}
void ItemSlider::DoCreateThumb()
{
    thumb->setContentSize(GetThumbSize(GetControlContentSize()));
    thumb->setBackGroundImage("sliderHAT.png");
    const int bitmapOffsetLeft=22;
    const int bitmapOffsetTop=13;
    const int bitmapOffsetRight=7;
    const int bitmapOffsetBottom=12;
    cocos2d::Rect r(bitmapOffsetLeft, bitmapOffsetTop, thumb->getBackGroundImageTextureSize().width-bitmapOffsetLeft-bitmapOffsetRight, thumb->getBackGroundImageTextureSize().height-bitmapOffsetTop-bitmapOffsetBottom);
    thumb->setBackGroundImageScale9Enabled(true);
    thumb->setBackGroundImageCapInsets(r);
}

void ItemSlider::InitLayoutOrientation(cocos2d::Vec2 rotationCenter)
{
    if (!slideBar) return;
    if ((GetLayoutManager()->IsVertical() && slideBar->getRotation()==0)  ||
        (!GetLayoutManager()->IsVertical() && slideBar->getRotation()!=0))
        return;
    int rotation=GetLayoutManager()->IsVertical()?0:90;

    thumb->setRotation(rotation);
    slideBar->setRotation(rotation);
    slideBarOff->setRotation(rotation);
    
    setPosition(Vec2(rotationCenter.x-getContentSize().width/2.0, rotationCenter.y+getContentSize().height/2.0));
}

void ItemSlider::DoSetContentSize(cocos2d::Size contentSize)
{
    cocos2d::Size temp=contentSize;
    if (slideBar)
    {
        cocos2d::Size barsSize(temp.height/3.0, temp.width);
        if (GetLayoutManager()->IsVertical())
            barsSize=cocos2d::Size(temp.width/3, temp.height);
            
        slideBar->setContentSize(barsSize);
        slideBarOff->setContentSize(cocos2d::Size(barsSize.width, slideBarOff->getContentSize().height));
    }
    if (thumb)
        thumb->setContentSize(GetThumbSize(temp));
    
    
    SetPositionOfValueDependentComponent();
}

void ItemKnob::DoSetContentSize(cocos2d::Size contentSize)
{
    cocos2d::Size newSize(GetThumbSize(contentSize));
    if (points)
    {
        points->setContentSize(newSize);
        points->setPosition(Vec2(0,newSize.height));
    }
    if (knob)
    {
        knob->setContentSize(newSize);
        knob->setPosition(Vec2(0,newSize.height));
    }
    for (int i=0;i<onPoints.size();++i)
    {
        onPoints[i]->setContentSize(newSize);
        onPoints[i]->setPosition(Vec2(0,newSize.height));
    }
    ItemSlider::DoSetContentSize(contentSize);
}

cocos2d::Size ItemKnob::CalculateNewItemBaseSize(int magValue)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.height+=magValue;
    baseSize.width+=magValue;
    return baseSize;
}

void ItemSlider::SetPositionOfValueDependentComponent()
{
    if (NULL==thumb) return;
    float offset =	thumb->getContentSize().height; //thumb has same size in both orientation, beacase we simply rotate it
    float size=GetLayoutManager()->IsVertical()?GetControlContentSize().height-offset:GetControlContentSize().width-offset;
    float position=(GetControlUnit()->GetNormalizedValue()*size)+offset/2.0;
    const int bitmapThumbXOffset=-8;
    if (GetLayoutManager()->IsVertical())
    {
        thumb->setPosition(Vec2(GetControlContentSize().width/2.0+bitmapThumbXOffset,position));
        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().height-position+2.0));
        slideBarOff->setPosition(Vec2(GetControlContentSize().width/2.0,position+slideBarOff->getContentSize().height/2.0));
        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    }
    else
    {
        thumb->setPosition(Vec2(position,GetControlContentSize().height/2.0-bitmapThumbXOffset));
        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().width-position+2));
        slideBarOff->setPosition(Vec2(position+slideBarOff->getContentSize().height/2.0, GetControlContentSize().height/2.0));
        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0/*-slideBar->getContentSize().width/2*/, GetControlContentSize().height/2.0));
    }
}

//bool ItemSlider::IsVertical()
//{
//	return isVertical;
//}

/*void ItemSlider::SetValue(float _value)
{
    GetControlUnit()->OnTouch(_value);
	//#ifdef DEBUG
    printf("KNOB VALUE %d\n", _value);
    //#endif
    SetThumbPosition();
}*/

//void ItemSlider::SetRange(int _min, int _max)
//{
//	GetControlUnit()->SetRange(_min,_max);
//    SetPositionOfValueDependentComponent();
//    //SetValue(GetValue());
//}

void ItemSlider::Init()
{
    DEFAULT_NAME("Slider")
}
void ItemWheel::Init()
{
    DEFAULT_NAME("Switch")
}
void ItemSlider::Create()
{
    Init();
    GetLayoutManager()->SetVertical(true);
    slideBar = Layout::create();
    control->addChild(slideBar);
    slideBar->setTouchEnabled(true);
   // slideBar->setAnchorPoint(Vec2(0,1));
    slideBar->setAnchorPoint(Vec2(0.5,0.5));
    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    //slideBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    //slideBar->setBackGroundColor(Color3B::BLACK);
    slideBar->setBackGroundImage("sliderBARStripe.png");
    const int bitmapcapInsetOffset=5;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, slideBar->getBackGroundImageTextureSize().width, slideBar->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(GetControlContentSize().width/3, GetControlContentSize().height);
    slideBar->setBackGroundImageScale9Enabled(true);
    slideBar->setBackGroundImageCapInsets(rr);
    slideBar->setContentSize(s);
    slideBar->setPosition(Vec2(GetControlContentSize().width/2/*-slideBar->getContentSize().width/2*/, GetControlContentSize().height/2));
    
    
    slideBarOff = Layout::create();
    control->addChild(slideBarOff);
    slideBarOff->setTouchEnabled(true);
   // slideBarOff->setAnchorPoint(Vec2(0,1));
    slideBarOff->setAnchorPoint(Vec2(0.5,0.5));
    slideBarOff->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    slideBarOff->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //slideBarOff->setBackGroundColor(Color3B::BLACK);
//    slideBarOff->setBackGroundImageScale9Enabled(true);
//    slideBarOff->ignoreContentAdaptWithSize(true);
    slideBarOff->setBackGroundImage("sliderBAR.png");
    slideBarOff->setBackGroundImageScale9Enabled(true);
    slideBarOff->setBackGroundImageCapInsets(rr);
    slideBarOff->setContentSize(cocos2d::Size(s.width, s.height+2));
    slideBarOff->setPosition(Vec2(GetControlContentSize().width/2/*-slideBarOff->GetControlContentSize().width/2*/, 2+GetControlContentSize().height/2));
    
    //InitSliderLayout();

    CreateThumb();
}

void ItemSlider::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    ItemBase::OnItemTouchBegan(widget, type);
    //dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
    //dragStartPos=dragPosUpdated=thumb->getWorldPosition();
    //if (callback.get())
    //    callback->OnItemTouchBegan();

//    GetControlUnit()->OnTouch( ControlUnit::TouchDown,
//        		GetControlUnit()->GetNormalizedValue());
    // control unit wire doesn't really need this
    // but dsp unit needs this for activate/deactivate

}
void ItemWheel::OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    GetControlUnit()->OnTouch(ControlUnit::TouchMove,0.5f);
    SetPositionOfValueDependentComponent();
    ItemSlider::OnItemTouchEnded(widget, type);
}
void ItemSlider::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;
    ItemBase::OnItemTouchEnded(widget, type);
    //if (callback.get())
    //    callback->OnItemTouchEnded();

//    GetControlUnit()->OnTouch( ControlUnit::TouchUp,
//        		GetControlUnit()->GetNormalizedValue());
    
//    if (widget->getParent()==control)
//        NotifyEvent(SCDFC_EVENTS_Move_Item);
    // control unit wire doesn't really need this
    // but dsp unit needs this for activate/deactivate

}

#define RangeRadStart 4.27
#define RangeRadEnd 5.4105206811824216884634413823147
#define PHI 3.1415926535897932384626433832795

void ItemKnob::AngularMode(Widget *knob)
{
    Vec2 touchWorldPos=knob->getTouchMovePosition();
    Node *parent=knob->getParent();
    
    static double AngleConstantOffset=PHI/2-RangeRadStart;
    
    Vec2 center(parent->getContentSize().width/2, parent->getContentSize().height/2);

    Vec2 touchPos=parent->convertToNodeSpace(touchWorldPos);
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
//    long temp=(max-min)-long(min+(max-min)*val);
//    long temp=(max-min)-long(min+(max-min)*val);
//    if(abs(temp-value)<((max-min)*2/3))
//        SetValue(temp);
    
    double temp=1.0-val;
    if(abs(temp-GetControlUnit()->GetNormalizedValue())<(2.0/3.0))
    {
        temp=fmin(1,fmax(temp,0));
        GetControlUnit()->OnTouch(ControlUnit::TouchMove,temp);
        SetPositionOfValueDependentComponent();
    }
    
}

Vec2 ItemSlider::OnMove(Widget *widget)
{
//    Vec2 limits=convertToWorldSpace(Vec2(_anchorPoint.x * _contentSize.width, _anchorPoint.y * _contentSize.height));//getWorldPosition();
    Node *parent=widget->getParent();
    Vec2 limits=parent->convertToWorldSpace(Vec2(parent->getAnchorPoint().x * parent->getContentSize().width, parent->getAnchorPoint().y * parent->getContentSize().height));
    Vec2 touchPos=widget->getTouchMovePosition();
    
    if (!IsKnob())
    {
        if (GetLayoutManager()->IsVertical())
            touchPos.y=fmin(fmax(touchPos.y,limits.y-parent->getContentSize().height),limits.y);
        else
            touchPos.x=fmin(fmax(touchPos.x,limits.x),limits.x+parent->getContentSize().width);
    }
    
    float distanceX=0, distanceY=0;
    
    cocos2d::Rect rItem(parent->getPositionX(), parent->getPositionY()-parent->getContentSize().height,parent->getContentSize().width, parent->getContentSize().height);
    
    if (!rItem.containsPoint(touchPos))
    {
        distanceX=abs(dragStartPos.x-touchPos.x);
        distanceY=abs(dragStartPos.y-touchPos.y);
    }
    
    float diff = 0;
//    float touchDistanceFromStart=IsVertical()?distanceX:distanceY;//sqrt(pow(distanceX,2)+pow(distanceY,2));
    if (GetLayoutManager()->IsVertical()){
        diff=touchPos.y-dragPosUpdated.y;
        float draggingVel=log(1+distanceX)+1;
        float sliderComp=thumb->getContentSize().height;
        if (IsKnob())
        {
            draggingVel=3*log(1+distanceX)+1;
            diff/=draggingVel;
            sliderComp=0;
        }
        diff/=(parent->getContentSize().height-sliderComp);
    }
    else{
        diff=touchPos.x-dragPosUpdated.x;
        float draggingVel=log(1+distanceY)+1;
        float sliderComp=thumb->getContentSize().height; //thumb has same size in both orientation, beacase we simply rotate it
        if (IsKnob())
        {
            draggingVel=3*log(1+distanceY)+1;
            diff/=draggingVel;
            sliderComp=0;
        }
        diff/=(parent->getContentSize().width-sliderComp);
    }
    
    float normValue=GetControlUnit()->GetNormalizedValue()+diff;
    normValue=fmin(1,fmax(0,normValue));
    LOGD("SLIDER VALUE %f\n", normValue);
    GetControlUnit()->OnTouch(ControlUnit::TouchMove,normValue);
    SetPositionOfValueDependentComponent();
    return touchPos;
}

Vec2 ItemKnob::OnMove(Widget *widget)
{
    Vec2 touchPos=widget->getTouchMovePosition();
    AngularMode(widget);
    //return ItemSlider::OnMove(widget) //Linear movement like slider
    return touchPos;
}

void ItemSlider::OnItemTouchMoved(Widget *widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (widget==slideBar) return;

    Vec2 touchPosNew=OnMove(widget);
    
// VALUE REMOVAL
//    if (callback.get())
//        callback->OnItemTouchMoved(value);
//    else
//        controlUnit->OnTouch(ControlUnit::TouchDown,value);
    
    dragPosUpdated=touchPosNew;

    if (widget->getParent()==control)
        NotifyEvent(SCDFC_EVENTS_Move_Item);
}

cocos2d::Size ItemSlider::GetStaticBaseSize()
{
//    if (IsVertical())
        return GetBaseSize();
//    return SLIDER_HORZ_SIZE_BASE;
}

void ItemSlider::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    slideBar->setColor(Colors::Instance()->GetItemsColor(colorIndex));
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
    int selectedPoint = 16 * GetControlUnit()->GetNormalizedValue();

	for (int i=0;i<onPoints.size();++i)
        onPoints[i]->setVisible(false);
    onPoints[selectedPoint]->setVisible(true);
}

void ItemKnob::SetPositionOfValueDependentComponent()
{
    if (NULL==thumb) return;
    thumb->setPosition(Vec2(GetControlContentSize().width/2.0,GetControlContentSize().height/2.0));
    float rotation= 270.0 * GetControlUnit()->GetNormalizedValue();
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
        control->addChild(point,1);
        point->setVisible(false);
        point->setAnchorPoint(Vec2(0,1));
        point->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
        point->setBackGroundImage(str);
        cocos2d::Rect r(0, 0, point->getBackGroundImageTextureSize().width, point->getBackGroundImageTextureSize().height);
        point->setBackGroundImageScale9Enabled(true);
        point->setBackGroundImageCapInsets(r);
        point->setContentSize(GetControlContentSize());
        point->setPosition(Vec2(0, GetControlContentSize().height));
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
    thumb->setContentSize(GetThumbSize(GetControlContentSize()));
    
    knob  = Layout::create();
    control->addChild(knob,1);
    //knob->setTouchEnabled(true);
    knob->setAnchorPoint(Vec2(0,1));
    knob->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    knob->setBackGroundImage("knobVOLKNOB.png");
    const int bitmapcapInsetOffset=0;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, knob->getBackGroundImageTextureSize().width, knob->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(GetControlContentSize().width, GetControlContentSize().height);
    knob->setBackGroundImageScale9Enabled(true);
    knob->setBackGroundImageCapInsets(rr);
    knob->setContentSize(s);
    knob->setPosition(Vec2(0, GetControlContentSize().height));
    
    points = Layout::create();
    control->addChild(points);
    points->setAnchorPoint(Vec2(0,1));
    points->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    points->setBackGroundImage("knobVOL0.png");
    points->setBackGroundImageScale9Enabled(true);
    points->setBackGroundImageCapInsets(rr);
    points->setContentSize(s);
    points->setPosition(Vec2(0, GetControlContentSize().height));
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
    GetLayoutManager()->SetVertical(true);
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

void ItemPad::Init()
{
    DEFAULT_NAME("Pad")
}

void ItemSwitch::Init()
{
    checked=0;
    DEFAULT_NAME("Swicth")
}

void ItemPad::Create()
{
    Init();
    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
    pad->loadTextures("padDefault.png", "padHover.png","padDefault.png");
    pad->setAnchorPoint(Vec2(0,1));
    pad->setPosition(Vec2(0,GetControlContentSize().height));
    pad->setContentSize(GetControlContentSize());
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    cocos2d::Rect rc(10,9,110,113);
    pad->setCapInsets(rc);
    control->addChild(pad,2);
}

void ItemPad::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    pad->setColor(Colors::Instance()->GetItemsColor(colorIndex));
    //    setColor(Colors::Instance()->GetItemsColor(colorIndex));
}

//void ItemPad::setContentSize(const cocos2d::Size &contentSize)
//{
//    Node::setContentSize(contentSize);
//    if (NULL==pad) return;
//    pad->setContentSize(contentSize);
//    pad->setPosition(Vec2(0,contentSize.height));
//}

void ItemPad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchBegan(widget,type);
    GetControlUnit()->OnTouch(ControlUnit::TouchDown,1);
    LOGD("%s sends %d with velocity %d \n", GetName().c_str(), GetControlUnit()->GetSender()->GetMidiMessageType(), GetControlUnit()->GetValue());
}

void ItemPad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    GetControlUnit()->OnTouch(ControlUnit::TouchUp,0.0);
    LOGD("%s sends %d with velocity %f \n", GetName().c_str(), GetControlUnit()->GetSender()->GetMidiMessageType(), 0.f);
}

void ItemPad::DoSetContentSize(cocos2d::Size contentSize)
{
    if (pad)
    {
        pad->setContentSize(contentSize);
        pad->setPosition(Vec2(0,contentSize.height));
    }
}

cocos2d::Size ItemPad::CalculateNewItemBaseSize(int magValue)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.height+=magValue;
    baseSize.width+=magValue;
    return baseSize;
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

    int numPadsRow=GetControlContentSize().width/padsWidth;
    int numPadsColumn=GetControlContentSize().height/padsHeight;
    for (int column=0;column<numPadsColumn;++column)
    {
        for (int row=0;row<numPadsRow;++row)
        {
            padIndex=0;
            cocos2d::Rect r(row*padsWidth,GetControlContentSize().height-column*padsHeight,padsWidth,padsHeight);
            ItemPad *pad=(ItemPad*)ItemBase::CreateItem<ItemPad>();
            pad->setPosition(r.origin);
            pads.push_back(pad);
            control->addChild(pad);
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

void ItemKeyboard::DoSetContentSize(cocos2d::Size contentSize)
{

    if (NULL==keysHandle) return;
//    float offset =	thumb->getContentSize().height; //thumb has same size in both orientation, beacase we simply rotate it
//    float size=GetLayoutManager()->IsVertical()?GetControlContentSize().height-offset:GetControlContentSize().width-offset;
//    float position=(GetControlUnit()->GetNormalizedValue()*size)+offset/2.0;
//    const int bitmapThumbXOffset=-8;
    if (GetLayoutManager()->IsVertical())
    {
        keysHandle->setContentSize(cocos2d::Size(contentSize.height, contentSize.width));
        
//        keysHandle->setPosition(Vec2(GetControlContentSize().width/2.0+bitmapThumbXOffset,position));
//        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().height-position+2.0));
//        slideBarOff->setPosition(Vec2(GetControlContentSize().width/2.0,position+slideBarOff->getContentSize().height/2.0));
//        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    }
    else
    {
        keysHandle->setContentSize(contentSize);

//        thumb->setPosition(Vec2(position,GetControlContentSize().height/2.0-bitmapThumbXOffset));
//        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().width-position+2));
//        slideBarOff->setPosition(Vec2(position+slideBarOff->getContentSize().height/2.0, GetControlContentSize().height/2.0));
//        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0/*-slideBar->getContentSize().width/2*/, GetControlContentSize().height/2.0));
    }
    keysHandle->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
}

void ItemKeyboard::Create()
{
    DEFAULT_NAME("Keyboard")
    GetLayoutManager()->SetVertical(false);
    currentOctave=4;
    selectedKey=-1;
    keysHandle = Layout::create();
    control->addChild(keysHandle);
    keysHandle->setTouchEnabled(true);
   // keysHandle->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    keysHandle->setAnchorPoint(Vec2(0.5,0.5));
    keysHandle->setContentSize(GetControlContentSize());
    keysHandle->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    
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
    blackKeyPressed->setScale(GetControlContentSize().width/r.size.width, GetControlContentSize().height/r.size.height);
    blackKeyPressed->setVisible(false);
    keysHandle->addChild(whiteKeyPressed);
    whiteKeyPressed->setAnchorPoint(Vec2(0,1));
    whiteKeyPressed->setScale(GetControlContentSize().width/r.size.width, GetControlContentSize().height/r.size.height);
    whiteKeyPressed->setVisible(false);
}

const float numKeyboardKeys=12.0;

bool ItemKeyboard::UpdateSelectedKey(Widget* widget, bool onMoving)
{
    Node *parent=widget->getParent();
    float mockWidth=GetLayoutManager()->IsVertical() ? parent->getContentSize().height : parent->getContentSize().width;
    float mockHeight=GetLayoutManager()->IsVertical() ? parent->getContentSize().width : parent->getContentSize().height;
    
    const float velo=127.0;
    float keyWidth=(mockWidth-(2*leftBitmapOffsetPercentageForPressedKey*mockWidth))/numKeyboardKeys;
    Vec2 nodeCoord=widget->convertToNodeSpace(dragPosUpdated);
    int lastSelectedKey=selectedKey;
    selectedKey=fmax(0,fmin(11,(nodeCoord.x-(leftBitmapOffsetPercentageForPressedKey*mockWidth))/keyWidth));
    if (lastSelectedKey==selectedKey && onMoving)
        return false;
    int vel=velo*(mockHeight-nodeCoord.y)/mockHeight;
    
    //SetValue(vel);
    GetControlUnit()->GetSender()->SetMidiPitch(numKeyboardKeys*currentOctave+selectedKey);
    GetControlUnit()->OnTouch(ControlUnit::TouchDown,vel/127.0);
    // VALUE REMOVAL
    //controlUnit->SendValue(vel);
    
    if (selectedKey==1||selectedKey==3||selectedKey==6||selectedKey==8||selectedKey==10)
    {

        blackKeyPressed->setPosition(Vec2(leftBitmapOffsetPercentageForPressedKey*mockWidth+keyWidth*selectedKey,mockHeight-(topBitmapOffsetPercentageForPressedKey*mockHeight)));
        blackKeyPressed->setVisible(true);
        whiteKeyPressed->setVisible(false);
    }
    else
    {
        whiteKeyPressed->setPosition(Vec2(leftBitmapOffsetPercentageForPressedKey*mockWidth+keyWidth*selectedKey,mockHeight-(topBitmapOffsetPercentageForPressedKey*mockHeight)));
        blackKeyPressed->setVisible(false);
        whiteKeyPressed->setVisible(true);
    }
    return true;
    LOGD("Keyboard sends MIDI note %d with vel %d\n", controlUnit->GetSender()->GetMidiPitch(), vel);
}

void ItemKeyboard::InitLayoutOrientation(cocos2d::Vec2 rotationCenter)
{
    if (!keysHandle) return;
    if ((!GetLayoutManager()->IsVertical() && keysHandle->getRotation()==0)  ||
        (GetLayoutManager()->IsVertical() && keysHandle->getRotation()!=0))
        return;
    int rotation=GetLayoutManager()->IsVertical()?-90:0;

    keysHandle->setRotation(rotation);
    setPosition(Vec2(rotationCenter.x-getContentSize().width/2.0, rotationCenter.y+getContentSize().height/2.0));
}

void ItemKeyboard::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchBegan(widget, type);
    UpdateSelectedKey(widget, false);
}

void ItemKeyboard::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchEnded(widget, type);
    selectedKey=-1;
    whiteKeyPressed->setVisible(false);
    blackKeyPressed->setVisible(false);
    GetControlUnit()->OnTouch(ControlUnit::TouchUp,0);
}

void ItemKeyboard::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    ItemBase::OnItemTouchMoved(widget, type);
    bool notify=UpdateSelectedKey(widget, true);
    if (notify && widget->getParent()==control)
        NotifyEvent(ScdfCtrl::SCDFC_EVENTS_Move_Item);
}

void ItemSwitch::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (checked) return;
    ItemPad::OnItemTouchBegan(widget,type);
    pad->loadTextureNormal("padHover.png");
    
}


void ItemSwitch::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!checked)
        checked=true;
    else{
        ItemPad::OnItemTouchEnded(widget,type);
        pad->loadTextureNormal("padDefault.png");
        checked=false;
    }
}

template ItemBase *ItemBase::CreateItem<ItemSlider>();
template ItemBase *ItemBase::CreateItem<ItemPad>();
template ItemBase *ItemBase::CreateItem<ItemKnob>();
template ItemBase *ItemBase::CreateItem<ItemKeyboard>();
template ItemBase *ItemBase::CreateItem<ItemMultipad>();
template ItemBase *ItemBase::CreateItem<ItemSwitch>();
template ItemBase *ItemBase::CreateItem<ItemWheel>();
