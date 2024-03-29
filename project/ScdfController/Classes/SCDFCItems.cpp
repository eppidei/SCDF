//
//  SCDFCItems.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 18/11/14.
//
//

#include "SCDFCItems.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "MainScene.h"
#include "ControlUnit.h"
#include "MultiSender.h"
#include "ScdfSensorAPI.h"
#include "InAppPurchase.h"

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

#define LABEL_X_OFFSET MainScene::GetUnityBase()/3.0

void ItemBase::CreateItemBaseElements()
{
    cocos2d::Rect lRect(0,getContentSize().height,getContentSize().width,ITEMS_LABEL_HEIGHT);
    label=TextWithBackground::CreateText(-1,lRect, name, Colors::Instance()->GetFontPath(Colors::FontsId::ItemLabel),Colors::Instance()->GetFontSize(Colors::FontsId::ItemLabel));
    label->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::NONE);
    label->SetTextColor(Color3B::BLACK);
    label->setTouchEnabled(true);
        label->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    addChild(label,1);
    
    controlImage=Layout::create();
    controlImage->setAnchorPoint(Vec2(0,1));
        controlImage->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    addChild(controlImage,1);
    controlImage->setTouchEnabled(true);
    SetControlModeImage();
    
    control=Layout::create();
    addChild(control,1);
    control->setAnchorPoint(Vec2(0,1));
    control->setContentSize(getContentSize());
    control->setPosition(Vec2(0,getContentSize().height));
    control->setTouchEnabled(true);
    control->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
}

void ItemBase::PlaceItemBaseElements()
{
    if (GetLayoutManager()->ControlIconOnTop(getContentSize()))
    {
        controlImage->setPosition(Vec2(getContentSize().width/2.0-controlImage->getContentSize().width/2.0,getContentSize().height));
        control->setPosition(Vec2(0,getContentSize().height-ITEMS_LABEL_HEIGHT));
        label->setPosition(Vec2(getContentSize().width/2.0-label->getContentSize().width/2.0,getContentSize().height-ITEMS_LABEL_HEIGHT-control->getContentSize().height-LABEL_X_OFFSET));
    }
    else
    {
        float itemInfoWidth=controlImage->getContentSize().width+label->getContentSize().width+LABEL_X_OFFSET;
        control->setPosition(Vec2(0,getContentSize().height));
        controlImage->setPosition(Vec2(getContentSize().width/2.0-itemInfoWidth/2.0,getContentSize().height-control->getContentSize().height-LABEL_X_OFFSET));
        label->setPosition(Vec2(getContentSize().width/2.0-itemInfoWidth/2.0+controlImage->getContentSize().width+LABEL_X_OFFSET,getContentSize().height-control->getContentSize().height-LABEL_X_OFFSET));
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

ItemBase *ItemBase::CreateItem(int id)
{
	switch (id) {
		case ITEM_SLIDER_ID:	return ItemBase::CreateItem<ItemSlider>();
		case ITEM_PAD_ID:		return ItemBase::CreateItem<ItemPad>();
		case ITEM_KNOB_ID:		return ItemBase::CreateItem<ItemKnob>();
		case ITEM_SWITCH_ID:	return ItemBase::CreateItem<ItemSwitch>();
		case ITEM_KEYBOARD_ID:	return ItemBase::CreateItem<ItemKeyboard>();
		case ITEM_MULTIPAD_ID:	return ItemBase::CreateItem<ItemMultipad>();
		case ITEM_WHEEL_ID:		return ItemBase::CreateItem<ItemWheel>();
		default: return NULL;
	}
}

void CheckForPurchase(ControlUnit *unit)
{
    if (unit->GetType()==ControlUnit::Wire) return;

    if (CheckIsInAppPurchasedNoPrompt((PurchaseProductIndex)unit->GetType())) return;
    
    unit->GetSender()->SetOscEnabled(false);
    unit->GetSender()->SetMidiOutIndex(-1);
}

void ItemBase::CheckForSensorExists()
{
    if (GetControlUnit()->GetType()!=ControlUnit::Proximity) return;
    
    if (!scdf::theSensorAPI()->SensorExists(scdf::Proximity))
    {
        ModalPanel::CreateModalPanel("Proximity sensor isn't available on this device");
        ChangeControlUnit(ControlUnit::Wire);
    }
}

ItemBase* ItemBase::DeserializeItem(SerializableItemData* sitem)
{
	ItemBase* i = CreateItem(sitem->id);

	i->SetName(sitem->name);
	i->SetColor((Colors::ItemsColorsId)sitem->color);
	i->GetLayoutManager()->SetMagValue(sitem->magValue);
	i->GetLayoutManager()->SetVertical(sitem->isVertical);
    i->setPosition(Vec2(sitem->x,sitem->y));
    
    i->SetGroupID(sitem->groupId);
    i->SetMaster(sitem->isMaster);
    i->setPosition(Vec2(sitem->x,sitem->y));

	i->SetControlUnit(sitem->unit);
    i->CheckForSensorExists();

    CheckForPurchase(i->GetControlUnit());
    
    if (i->GetID()==ITEM_KEYBOARD_ID)
        dynamic_cast<ItemKeyboard*>(i)->SetCurrentOctave(sitem->octave);
    
    i->UpdateUI();
    return i;
}

template <class ItemType> ItemBase *ItemBase::CreateItem()
{
    ItemBase *item=ItemType::create();

    item->setAnchorPoint(Vec2(0,1));
    item->CreateItemBaseElements();
    item->GetLayoutManager()->SetMagValue(0);
    item->Create();
    item->SetColor(Colors::ItemsColorsId::GreenUI);
    return item;
}

void ItemBase::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    Layout::visit(renderer, parentTransform, parentFlags);
    if (updateUIDeferred)
    {
        UpdateUI();
        updateUIDeferred=false;
    }
}

ItemBase::ItemBase() : control(NULL), label(NULL), controlImage(NULL), groupId(-1), isMaster(false), updateUIDeferred(false)
{
	ChangeControlUnit(ControlUnit::Wire);
    updater.reset(new ItemUIUpdater(this));
    layoutManager.reset(new ItemLayoutManager(this));
}

ItemBase::~ItemBase()
{
    controlUnit->SetInterface(NULL);
    SetControlUnit(NULL);
    control->removeAllChildren();
    removeAllChildren();
}

void ItemBase::SetControlModeImage()
{
    if (!controlImage) return;
    switch(controlUnit->GetType())
    {
        case ControlUnit::Wire:
            controlImage->setBackGroundImage("modeTouch.png");
            break;
        case ControlUnit::Blow:
            controlImage->setBackGroundImage("modeBlow.png");
            break;
        case ControlUnit::Snap:
            controlImage->setBackGroundImage("modeSnap.png");
            break;
        case ControlUnit::Proximity:
            controlImage->setBackGroundImage("modeGesture.png");
            break;
        default:
            break;
    }
//    controlImage->setAnchorPoint(Vec2(0,1));
    float iconDim=ITEMS_LABEL_HEIGHT-LABEL_X_OFFSET;
    cocos2d::Rect rr(0, 0, 200/*controlImage->getBackGroundImageTextureSize().width, controlImage->getBackGroundImageTextureSize().height*/,200);
    controlImage->setBackGroundImageScale9Enabled(true);
    controlImage->setBackGroundImageCapInsets(rr);
    controlImage->setContentSize(cocos2d::Size(iconDim, iconDim));
//    controlImage->setScale(iconDim/controlImage->getTexture()->getContentSizeInPixels().width, iconDim/controlImage->getTexture()->getContentSizeInPixels().height);
}
void ItemBase::ChangeControlUnit(ControlUnit::Type t)
{
    if (GetControlUnit()&&GetControlUnit()->GetType()==t) return;
    SetControlUnit(ControlUnit::Create(t));
}

void ItemBase::SetControlUnit(ControlUnit* cu)
{
    //scdf::theSensorAPI()->DetachHarvesterListener(GetControlUnit());
    ControlUnit::DetachUnit(GetControlUnit());
	controlUnit.reset(cu); // item becomes the owner of the ctrl unit
    if (NULL==controlUnit.get()) return;
    
    controlUnit->SetInterface(updater.get());

    ControlUnit::AttachUnit(GetControlUnit());
    SetControlModeImage();
    SetControlUnitReceiverType();
}

cocos2d::Size ItemSlider::CalculateNewItemBaseSize(int magValue)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.height+=magValue;
    baseSize.width=(int)((baseSize.width*baseSize.height)/GetStaticBaseSize().height);
    return baseSize;
}

cocos2d::Size ItemSwitch::CalculateNewItemBaseSize(int magValue)
{
    cocos2d::Size baseSize=GetStaticBaseSize();
    baseSize.width+=magValue;
    baseSize.height=(int)((baseSize.width*baseSize.height)/GetStaticBaseSize().width);
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
    r.origin=w->convertToWorldSpace(r.origin);
    if (collision || !w->IsInsideVisibleSpace(r))
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
            wPanel->OnItemTouchBegan(this, widget, type);
            break;
        case Widget::TouchEventType::MOVED:
            wPanel->OnItemTouchMoved(this, widget, type);
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            wPanel->OnItemTouchEnded(this, widget, type);
            break;
        default:
            break;
    }
}

void ItemBase::Select(bool select)
{
    if (select)
    {
        label->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
        label->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    }
    else
        label->setBackGroundColorType(Layout::BackGroundColorType::NONE);
}

bool ItemBase::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
   // if (widget==controlImage || widget==label) return false;
    
    //if (widget->getParent()==control)
        NotifyEvent(SCDFC_EVENTS_Select_Item);
    
    if (controlUnit->GetType()!=ControlUnit::Wire) return false;
    
    dragStartPos=dragPosUpdated=widget->getTouchBeganPosition();
    return true;
}
bool ItemBase::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    //if (widget==controlImage || widget==label) return false;
    if (controlUnit->GetType()!=ControlUnit::Wire) return false;
    dragPosUpdated=widget->getTouchMovePosition();
    return true;
}

bool ItemBase::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    //if (widget==controlImage || widget==label) return false;
    NotifyEvent(SCDFC_EVENTS_Select_Item);
    if (controlUnit->GetType()!=ControlUnit::Wire) return false;
    return true;
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
    UpdateUI();
}

cocos2d::Size ItemSlider::GetThumbSize(cocos2d::Size currentSize)
{
    float size=fmin(currentSize.height,currentSize.width);
//    float size=fmin(GetBaseSize().height,GetBaseSize().width);
    cocos2d::Size s;
    if (!IsPitchWheel())
        s=Size(size+35, size+5);
    else
        s=Size(size+10, size+20);
    return s;
}
void ItemSlider::DoCreateThumb()
{
    thumb->setContentSize(GetThumbSize(GetControlContentSize()));
    if (IsPitchWheel())
        thumb->setBackGroundImage("pitchWheelHat.png");
    else
        thumb->setBackGroundImage("sliderHAT.png");
    const int bitmapOffsetLeft=0;//64;
    const int bitmapOffsetTop=0;//38;
    const int bitmapOffsetRight=0;//20;
    const int bitmapOffsetBottom=0;//43;
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
        float reductionFactor=3.0;
        if (IsPitchWheel())
            reductionFactor=1.0;

        cocos2d::Size barsSize(temp.height/reductionFactor, temp.width);
        if (GetLayoutManager()->IsVertical())
            barsSize=cocos2d::Size(temp.width/reductionFactor, temp.height);
            
        slideBar->setContentSize(barsSize);
        slideBarOff->setContentSize(cocos2d::Size(barsSize.width, slideBarOff->getContentSize().height));
    }
    if (thumb)
        thumb->setContentSize(GetThumbSize(temp));
    
    
    UpdateUI();
}

void ItemKnob::DoSetContentSize(cocos2d::Size contentSize)
{
    cocos2d::Size newSize(GetThumbSize(contentSize));
//    if (points)
//    {
//        points->setContentSize(newSize);
//        points->setPosition(Vec2(0,newSize.height));
//    }
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

void ItemSlider::UpdateUI()
{
    SetPositionOfValueDependentComponent();
}

void ItemSlider::SetPositionOfValueDependentComponent()
{
    if (NULL==thumb) return;
    float offset =	thumb->getContentSize().height; //thumb has same size in both orientation, beacase we simply rotate it
    float size=GetLayoutManager()->IsVertical()?GetControlContentSize().height-offset:GetControlContentSize().width-offset;
    float position=(GetControlUnit()->GetNormalizedValue()*size)+offset/2.0;
    float slideBarOffOffset=2.0;
    if (IsPitchWheel())
        slideBarOffOffset=0;
    if (GetLayoutManager()->IsVertical())
    {
        thumb->setPosition(Vec2(GetControlContentSize().width/2.0,position));
        if (IsPitchWheel())
            position=0;
        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().height-position));
            slideBarOff->setPosition(Vec2(GetControlContentSize().width/2.0,slideBarOffOffset+position+slideBarOff->getContentSize().height/2.0));
        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    }
    else
    {
        thumb->setPosition(Vec2(position,GetControlContentSize().height/2.0));
        if (IsPitchWheel())
            position=0;
        slideBarOff->setContentSize(cocos2d::Size(slideBarOff->getContentSize().width,GetControlContentSize().width-position));
        slideBarOff->setPosition(Vec2(slideBarOffOffset+position+slideBarOff->getContentSize().height/2.0, GetControlContentSize().height/2.0));
        slideBar->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    }
}

void ItemSlider::Init()
{
    DEFAULT_NAME("Slider")
}
void ItemWheel::Init()
{
    DEFAULT_NAME("Wheel")
}
void ItemSlider::Create()
{
    Init();
    GetLayoutManager()->SetVertical(true);
    slideBar = Layout::create();
    control->addChild(slideBar);
    slideBar->setTouchEnabled(true);
    slideBar->setAnchorPoint(Vec2(0.5,0.5));
    slideBar->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));

    int bitmapcapInsetOffset=5;
    float reductionFactor=3.0;
    if (IsPitchWheel())
    {
        slideBar->setBackGroundImage("pitchWheelBaseBar.png");
        bitmapcapInsetOffset=0;
        reductionFactor=1.0;
    }
    else
        slideBar->setBackGroundImage("sliderBARStripe.png");
    
    cocos2d::Rect rr(0, bitmapcapInsetOffset, slideBar->getBackGroundImageTextureSize().width, slideBar->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    
    
    cocos2d::Size s(GetControlContentSize().width/reductionFactor, GetControlContentSize().height);
    slideBar->setBackGroundImageScale9Enabled(true);
    slideBar->setBackGroundImageCapInsets(rr);
    slideBar->setContentSize(s);
    slideBar->setPosition(Vec2(GetControlContentSize().width/2.0/*-slideBar->getContentSize().width/2*/, GetControlContentSize().height/2.0));
    
    slideBarOff=NULL;

    slideBarOff = Layout::create();
    control->addChild(slideBarOff);
    slideBarOff->setTouchEnabled(true);
    slideBarOff->setAnchorPoint(Vec2(0.5,0.5));
    slideBarOff->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    slideBarOff->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    if(IsPitchWheel())
        slideBarOff->setBackGroundImage("pitchWheelBase.png");
    else
        slideBarOff->setBackGroundImage("sliderBAR.png");
    slideBarOff->setBackGroundImageScale9Enabled(true);
    slideBarOff->setBackGroundImageCapInsets(rr);
    slideBarOff->setContentSize(cocos2d::Size(s.width, s.height));
    slideBarOff->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));

    CreateThumb();
}

bool ItemSlider::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
//    if (widget==slideBar) return false;
    return ItemBase::OnItemTouchBegan(widget, type);
}

bool ItemWheel::OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemSlider::OnItemTouchEnded(widget, type)) return false;
    GetControlUnit()->OnTouch(ControlUnit::TouchMove,0.5f);
    UpdateUI();
    
    if (widget->getParent()==control)
        NotifyEvent(SCDFC_EVENTS_Move_Item);
    
    return true;
}

bool ItemSlider::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    return ItemBase::OnItemTouchEnded(widget, type);
}

#define RangeRadStart 4.27
#define RangeRadEnd 5.4105206811824216884634413823147
#define PHI 3.1415926535897932384626433832795

void ItemKnob::AngularMode(Widget *knob)
{
    Vec2 touchWorldPos=knob->getTouchMovePosition();
    Node *parent=knob->getParent();
    
    static double AngleConstantOffset=PHI/2.0f-RangeRadStart;
    
    Vec2 center(parent->getContentSize().width/2.0, parent->getContentSize().height/2.0);

    Vec2 touchPos=parent->convertToNodeSpace(touchWorldPos);
    double x=touchPos.x;
    double  y=touchPos.y;
    
    double dx=x-center.x;
    double dy=-y+center.y;
    double angle=atan2(dy, dx);

    angle-=AngleConstantOffset;
    
    if(angle>3.0*PHI/2.0) angle-=2.0*PHI;
    if(angle<0&&angle<-PHI/2.0) angle+=2.0*PHI;
    
    angle=RangeRadStart-angle;
    double val=angle/RangeRadEnd;
    double temp=1.0-val;
    
    if(abs(temp-GetControlUnit()->GetNormalizedValue())<(2.0/3.0))
    {
        temp=fmin(1.0f,fmax(temp,0.f));
        GetControlUnit()->OnTouch(ControlUnit::TouchMove,temp);
        UpdateUI();
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
            draggingVel=3.0*log(1+distanceX)+1;
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
            draggingVel=3.0*log(1+distanceY)+1;
            diff/=draggingVel;
            sliderComp=0;
        }
        diff/=(parent->getContentSize().width-sliderComp);
    }
    
    float normValue=GetControlUnit()->GetNormalizedValue()+diff;
    normValue=fmin(1.f,fmax(0.f,normValue));
    LOGD("SLIDER VALUE %f\n", normValue);
    GetControlUnit()->OnTouch(ControlUnit::TouchMove,normValue);
    UpdateUI();
    return touchPos;
}

Vec2 ItemKnob::OnMove(Widget *widget)
{
    Vec2 touchPos=widget->getTouchMovePosition();
    AngularMode(widget);
    //return ItemSlider::OnMove(widget) //Linear movement like slider
    return touchPos;
}

bool ItemSlider::OnItemTouchMoved(Widget *widget, cocos2d::ui::Widget::TouchEventType type)
{
//    if (widget==slideBar) return false;
    Vec2 dragPosUpdatedOld=dragPosUpdated;
    if (!ItemBase::OnItemTouchMoved(widget, type)) return false;
    dragPosUpdated=dragPosUpdatedOld;
    Vec2 touchPosNew=OnMove(widget);
    dragPosUpdated=touchPosNew;

    if (widget->getParent()==control)
        NotifyEvent(SCDFC_EVENTS_Move_Item);
    
    return true;
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
        control->addChild(point,2);
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
    knob->setBackGroundImage("knobVOL0.png");
    const int bitmapcapInsetOffset=0;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, knob->getBackGroundImageTextureSize().width, knob->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(GetControlContentSize().width, GetControlContentSize().height);
    knob->setBackGroundImageScale9Enabled(true);
    knob->setBackGroundImageCapInsets(rr);
    knob->setContentSize(s);
    knob->setPosition(Vec2(0, GetControlContentSize().height));
    
//    points = Layout::create();
//    control->addChild(points);
//    points->setAnchorPoint(Vec2(0,1));
//    points->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
//    points->setBackGroundImage("knobVOL0.png");
//    points->setBackGroundImageScale9Enabled(true);
//    points->setBackGroundImageCapInsets(rr);
//    points->setContentSize(s);
//    points->setPosition(Vec2(0, GetControlContentSize().height));
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
    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
    pad->loadTextures("padDefault.png", "padDefault.png","padDefault.png");
    pad->setAnchorPoint(Vec2(0,1));
    pad->setPosition(Vec2(0,GetControlContentSize().height));
    pad->setContentSize(GetControlContentSize());
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    cocos2d::Rect rc(10,9,110,113);
    pad->setCapInsets(rc);
    control->addChild(pad,2);
}

void ItemSwitch::Init()
{
    //checked=0;
    DEFAULT_NAME("Swicth")
    GetLayoutManager()->SetVertical(false);
    backGround = Layout::create();
    control->addChild(backGround,1);
//    toggle->setTouchEnabled(true);
    // keysHandle->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    backGround->setAnchorPoint(Vec2(0.5,0.5));
//    backGround->setContentSize(GetControlContentSize());
//    toggle->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
    backGround->setBackGroundImage("switchBase.png");
    const int bitmapcapInsetOffset=0;
    cocos2d::Rect rr(0, bitmapcapInsetOffset, backGround->getBackGroundImageTextureSize().width, backGround->getBackGroundImageTextureSize().height-2*bitmapcapInsetOffset);
    cocos2d::Size s(GetControlContentSize().width, GetControlContentSize().height);
    backGround->setBackGroundImageScale9Enabled(true);
    backGround->setBackGroundImageCapInsets(rr);
    backGround->setContentSize(s);
    backGround->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));
    backGround->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));

    pad = Button::create();
    pad->setTouchEnabled(true);
    //pad->setScale9Enabled(true);
    pad->ignoreContentAdaptWithSize(false);
    pad->loadTextures("switchSlider.png", "switchSlider.png");
    pad->setAnchorPoint(Vec2(0,1));
    pad->setPosition(Vec2(0,GetControlContentSize().height));
    pad->setContentSize(cocos2d::Size(GetControlContentSize().width/2.0, GetControlContentSize().height));
    pad->addTouchEventListener(CC_CALLBACK_2(ItemBase::ItemsTouchCallback, this));
//    cocos2d::Rect rc(10,9,110,113);
//    pad->setCapInsets(rc);
    control->addChild(pad,2);
    
    UpdateUI();
}

void ItemPad::Create()
{
    
    Init();
    GetControlUnit()->SetMax(127);
    GetControlUnit()->SetMin(0);
}

void ItemPad::SetColor(Colors::ItemsColorsId colorIndex)
{
    ItemBase::SetColor(colorIndex);
    pad->setColor(Colors::Instance()->GetItemsColor(colorIndex));
}

bool ItemPad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchBegan(widget,type)) return false;
    GetControlUnit()->OnTouch(ControlUnit::TouchDown,1);
    LOGD("%s sends %d with velocity %d \n", GetName().c_str(), GetControlUnit()->GetSender()->GetMidiMessageType(), GetControlUnit()->GetValue());
    return true;
}

void ItemPad::UpdateUI()
{
    if (!GetControlUnit()) return;
    if (GetControlUnit()->GetNormalizedValue()>0)
        pad->loadTextureNormal("padHover.png");
    else
        pad->loadTextureNormal("padDefault.png");
}

bool ItemPad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchEnded(widget,type)) return false;
    GetControlUnit()->OnTouch(ControlUnit::TouchUp,0.0);
    LOGD("%s sends %d with velocity %f \n", GetName().c_str(), GetControlUnit()->GetSender()->GetMidiMessageType(), 0.f);
    return true;
}

void ItemSwitch::DoSetContentSize(cocos2d::Size contentSize)
{
    if (NULL==pad) return;
    if (GetLayoutManager()->IsVertical())
    {
        pad->setContentSize(cocos2d::Size(contentSize.height/2.0, contentSize.width));
        backGround->setContentSize(cocos2d::Size(contentSize.height, contentSize.width));
    }
    else
    {
        pad->setContentSize(cocos2d::Size(contentSize.width/2.0, contentSize.height));
        backGround->setContentSize(contentSize);
    }
    UpdateUI();
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

bool ItemMultipad::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchBegan(widget, type)) return false;
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return false;
    UpdateSelectedPadIndex(pad);
    ItemBase::OnItemTouchBegan(widget, type);
    pad->OnItemTouchBegan(widget, type);
    return true;
}

bool ItemMultipad::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchEnded(widget, type)) return false;
    ItemPad* pad=dynamic_cast<ItemPad*>(widget->getParent());
    if (NULL==pad) return false;
    pad->OnItemTouchEnded(widget, type);
    return true;
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
    if (GetLayoutManager()->IsVertical())
        keysHandle->setContentSize(cocos2d::Size(contentSize.height, contentSize.width));
    else
        keysHandle->setContentSize(contentSize);
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
    LOGD("Keyboard sends MIDI note %d with vel %d\n", controlUnit->GetSender()->GetMidiPitch(), vel);
    return true;
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

void ItemSwitch::InitLayoutOrientation(cocos2d::Vec2 rotationCenter)
{
    if (!backGround) return;
    if ((!GetLayoutManager()->IsVertical() && backGround->getRotation()==0)  ||
        (GetLayoutManager()->IsVertical() && backGround->getRotation()!=0))
        return;
    int rotation=GetLayoutManager()->IsVertical()?-90:0;
    
    backGround->setRotation(rotation);
    pad->setRotation(rotation);
    
    setPosition(Vec2(rotationCenter.x-getContentSize().width/2.0, rotationCenter.y+getContentSize().height/2.0));
}

bool ItemKeyboard::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchBegan(widget, type)) return false;
    UpdateSelectedKey(widget, false);
    if (widget->getParent()==control)
        NotifyEvent(ScdfCtrl::SCDFC_EVENTS_Move_Item);
    return true;
}

bool ItemKeyboard::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchEnded(widget, type)) return false;
    selectedKey=-1;
    whiteKeyPressed->setVisible(false);
    blackKeyPressed->setVisible(false);
    GetControlUnit()->OnTouch(ControlUnit::TouchUp,0);
    return true;
}

bool ItemKeyboard::OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchMoved(widget, type)) return false;
    bool notify=UpdateSelectedKey(widget, true);
    if (notify && widget->getParent()==control)
        NotifyEvent(ScdfCtrl::SCDFC_EVENTS_Move_Item);
    return true;
}

bool ItemSwitch::OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
   // if (checked)
    bool ret;
    if(GetControlUnit()->GetValue()>0)
    {
        ret= ItemPad::OnItemTouchEnded(widget,type);
    }
    else
   // if (!ItemPad::OnItemTouchBegan(widget,type)) return false;
        ret= ItemPad::OnItemTouchBegan(widget,type);
    UpdateUI();
//    if (!GetLayoutManager()->IsVertical())
//        pad->setPosition(Vec2(0,pad->getParent()->getContentSize().height));
//    else
//        pad->setPosition(Vec2(0,0));//pad->getParent()->getContentSize().height/2.0));
    return ret;
}

void ItemSwitch::UpdateUI()
{
    backGround->setPosition(Vec2(GetControlContentSize().width/2.0, GetControlContentSize().height/2.0));

    if (!GetLayoutManager()->IsVertical())
    {
        //if (checked)
        if(GetControlUnit()->GetValue()>0)
            pad->setPosition(Vec2(0,pad->getParent()->getContentSize().height));
        else
            pad->setPosition(Vec2(pad->getParent()->getContentSize().width/2.0, pad->getParent()->getContentSize().height));
    }
    else
    {
      //  if (checked)
        if(GetControlUnit()->GetValue()>0)
            pad->setPosition(Vec2(0,0));
        else
            pad->setPosition(Vec2(0, pad->getParent()->getContentSize().height/2.0));

    }
}

bool ItemSwitch::OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!ItemBase::OnItemTouchEnded(widget, type)) return false;
    
////    if (!checked)
////        checked=true;
////    else{
//    if(GetControlUnit()->GetValue()>0){
//        ItemPad::OnItemTouchEnded(widget,type);
//    //    checked=false;
//        UpdateUI();
//    }
    return true;
}

template ItemBase *ItemBase::CreateItem<ItemSlider>();
template ItemBase *ItemBase::CreateItem<ItemPad>();
template ItemBase *ItemBase::CreateItem<ItemKnob>();
template ItemBase *ItemBase::CreateItem<ItemKeyboard>();
template ItemBase *ItemBase::CreateItem<ItemMultipad>();
template ItemBase *ItemBase::CreateItem<ItemSwitch>();
template ItemBase *ItemBase::CreateItem<ItemWheel>();
