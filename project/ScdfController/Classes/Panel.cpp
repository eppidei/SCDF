//
//  Panel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#include "Panel.h"
#include "MainScene.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"
#include "LoadSavePanel.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

template PanelBase *PanelBase::CreatePanel<PropertiesPanel>(MainScene *main, cocos2d::Rect r);
template PanelBase *PanelBase::CreatePanel<LoadSavePanel>(MainScene *main, cocos2d::Rect r);

template <class PanelType> PanelBase *PanelBase::CreatePanel(MainScene *main, cocos2d::Rect r)
{
    PanelType *panel=PanelType::create();
    panel->InitWithContent(main,r);
    return panel;
}

void PanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(0, getInnerContainerSize().height), Vec2(getInnerContainerSize().width, 0), Color4F(((float)MAIN_BACK_COLOR.r)/255.f,((float)MAIN_BACK_COLOR.g)/255.f,((float)MAIN_BACK_COLOR.b)/255.f,1.0));
}

void PanelBase::EnableScrolling(bool enable)
{
    setDirection(enable?Direction::VERTICAL:Direction::NONE);
}

void PanelBase::PlaceSubPanels()
{
    Vector<Node*> _childrens = getChildren();
    float lastPosY=getInnerContainerSize().height;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (!(*it)->isVisible()) continue;
        lastPosY-=SUBPANEL_DISTANCE;
        (*it)->setPosition(Vec2(SUBPANEL_DISTANCE, lastPosY));
        lastPosY-=(*it)->getContentSize().height;
    }
}

void PanelBase::InitLayout()
{
    CalculateInnerHeight();
    PlaceSubPanels();
}

void PanelBase::CalculateInnerHeight()
{
    Vector<Node*> _childrens = getChildren();
    int innerHeight=0;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (!(*it)->isVisible()) continue;
        innerHeight+=SUBPANEL_DISTANCE;
        innerHeight+=(*it)->getContentSize().height;
    }
    innerHeight+=SUBPANEL_DISTANCE;
    setInnerContainerSize(cocos2d::Size(getContentSize().width,innerHeight));
}

void PanelBase::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
    setInertiaScrollEnabled(true);
    main->addChild(this);
    InitPanel();
}

void PanelBase::HideShow(PanelBase *substitute)
{
    MoveBy *action=NULL;
    CallFunc *callback=nullptr;
    if (NULL!=substitute)
        callback = CallFunc::create([substitute](){
            substitute->HideShow();
        });
    
    if (getPositionX()==0)
        action = MoveBy::create(0.1f, cocos2d::Vec2(-getContentSize().width, 0));
    else if (NULL==substitute)
        action = MoveBy::create(0.1f, cocos2d::Vec2(getContentSize().width, 0));
    
    if (NULL==action)
    {
        //Substitution case when panel to substitude is closed
        if (NULL!=substitute)
            substitute->HideShow();
        return;
    }
    
    if (nullptr!=callback){
        //Substitution case when panel to substitude is opened
        auto seq = Sequence::create(action, callback, NULL);
        runAction(seq);
    }
    else
        //No Substitution
        runAction(action);
}



void SubpanelBase::DropDownMenuCallbackSubPanel::OnSizeChanged(float oldSize, float newSize)
{
    parent->GetParent()->InitLayout();
}

void SubpanelBase::DropDownMenuCallbackSubPanel::OnSelectItem(DropDownMenu *menu)
{
    parent->OnDropDownSelectionChange(menu);
}

//void SubpanelBase::SubPanelItemCallback::OnItemTouchBegan()
//{
//    parent->GetParent()->EnableScrolling(false);
//}
//void SubpanelBase::SubPanelItemCallback::OnItemTouchMoved(int value)
//{
//    ItemBase *item=parent->GetParent()->GetSelectedItem();
//    if (NULL==item) return;
//    item->SetItemMultiply(value);
//}
//void SubpanelBase::SubPanelItemCallback::OnItemTouchEnded()
//{
//    parent->GetParent()->EnableScrolling(true);
//}

void SubpanelBase::CalculateHeight(bool realSize)
{
    int height=0;
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (!(*it)->isVisible() || (*it)->getPositionX()>getContentSize().width/2.0) continue;
       // if (realSize)
            height+=(*it)->getContentSize().height;
//        else
//            height+=SUBPANEL_ITEM_HEIGHT;//(*it)->getContentSize().height;
    }
    Resize(height);
}

void SubpanelBase::HideElement(Node *n, bool hide)
{
    n->setVisible(!hide);
    // CalculateHeight();
}

void SubpanelBase::InitChildrensVisibilityAndPos()
{
    CalculateHeight();
    PositionElements();
}

SubpanelBase::~SubpanelBase()
{
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
        removeChild(*it);
}

PanelBase *SubpanelBase::GetParent()
{
    return parent.get();
}

void SubpanelBase::Resize(float newHeight)
{
    auto modifyHeight = ActionTween::create(0.1, "height", getContentSize().height, newHeight);
    runAction(modifyHeight);
}

void SubpanelBase::updateTweenAction(float value, const std::string& key)
{
    if ("height"==key){
        setContentSize(cocos2d::Size(getContentSize().width, value));
        parent->InitLayout();
    }
    
}

void SubpanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F(((float)MAIN_BACK_COLOR_LIGHT.r)/255.f,((float)MAIN_BACK_COLOR_LIGHT.g)/255.f,((float)MAIN_BACK_COLOR_LIGHT.b)/255.f,1.0));
    if (flags&FLAGS_CONTENT_SIZE_DIRTY)
    {
        PositionElements();
        // parent->InitLayout();
        
    }
}

void SubpanelBase::InitWithContent(PanelBase *_parent, cocos2d::Size s)
{
    parent.reset(_parent);
    dropDownCallback.reset(new DropDownMenuCallbackSubPanel(this));
    //itemCallback.reset(new SubPanelItemCallback(this));
    setContentSize(s);
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
    //    setBackGroundColor(MAIN_BACK_COLOR);
    setAnchorPoint(Vec2(0,1));
    CreateControls();
   // CalculateHeight(true);
    PositionElements();
    InitChildrensVisibilityAndPos();
}

void SubpanelBase::TouchEventCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Node* node = dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            OnTouchEventBegan(node);
            break;
        case Widget::TouchEventType::MOVED:
            //OnTouchMoved(node);
            break;
        case Widget::TouchEventType::ENDED:
            OnTouchEventEnded(node);
            break;
        case Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}

void SubpanelBase::ListviewItemEvent(Ref *pSender, ListView::EventType type)
{
    ListView *list=dynamic_cast<ListView*>(pSender);
    switch (type)
    {
        case ListView::EventType::ON_SELECTED_ITEM_START:
            list->getItem(list->getCurSelectedIndex())->setHighlighted(true);
            OnListViewItemSelected(list);
            break;
        case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
            list->getItem(list->getCurSelectedIndex())->setHighlighted(true);
            
            break;
        default:
            break;
    }
}

void SubpanelBase::TextFieldEventCallback(Ref *pSender, TextField::EventType type)
{
    TextField *text=dynamic_cast<TextField*>(pSender);
    switch (type)
    {
        case TextField::EventType::DETACH_WITH_IME:
            OnTextInput(text);
            break;
        default:
            break;
    }
}

void SubpanelBase::CheckBoxEventCallback(Ref* pSender,CheckBox::EventType type)
{
    CheckBox *checkBox=dynamic_cast<CheckBox*>(pSender);
    OnCheckEvent(checkBox, type==CheckBox::EventType::SELECTED);
}