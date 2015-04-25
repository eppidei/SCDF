//
//  Panel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#include "Panel.h"
#include "MainScene.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "PropertiesPanel.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

template PanelBase *PanelBase::CreatePanel<PropertiesPanel>(MainScene *main, cocos2d::Rect r);
//template PanelBase *PanelBase::CreatePanel<LoadSavePanel>(MainScene *main, cocos2d::Rect r);

template <class PanelType> PanelBase *PanelBase::CreatePanel(MainScene *main, cocos2d::Rect r)
{
    PanelType *panel=PanelType::create();
    panel->InitWithContent(main,r);
    return panel;
}

//void PanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{
//    Color3B cc=Colors::Instance()->GetUIColor(Colors::Main_Background);
//    DrawPrimitives::drawSolidRect(Vec2(0, getInnerContainerSize().height), Vec2(getInnerContainerSize().width, 0), Color4F(((float)cc.r)/255.f,((float)cc.g)/255.f,((float)cc.b)/255.f,1.0));
//}

void PanelBase::EnableScrolling(bool enable)
{
    scrollView->setDirection(enable?cocos2d::ui::ScrollView::Direction::VERTICAL:cocos2d::ui::ScrollView::Direction::NONE);
}

void PanelBase::PlaceSubPanels()
{
//    const int offsety=100;
    Vector<Node*> _childrens = scrollView->getChildren();
    float lastPosY=scrollView->getInnerContainerSize().height;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (!(*it)->isVisible()) continue;
        (*it)->setPosition(Vec2(0, lastPosY));
        lastPosY-=(*it)->getContentSize().height;
        lastPosY-=SUBPANEL_DISTANCE;
    }
}

void PanelBase::InitLayout()
{
    CalculateInnerHeight();
    PlaceSubPanels();
}

void PanelBase::CalculateInnerHeight()
{
    Vector<Node*> _childrens = scrollView->getChildren();
    int innerHeight=0;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (dynamic_cast<SubpanelBase*>(*it)==NULL) continue;
        if (!(*it)->isVisible()) continue;
        innerHeight+=SUBPANEL_DISTANCE;
        innerHeight+=(*it)->getContentSize().height;
    }
    innerHeight+=SUBPANEL_DISTANCE;
    scrollView->setInnerContainerSize(cocos2d::Size(getContentSize().width,innerHeight));
}

void PanelBase::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    visible=false;
    parent=main;
    scrollView=cocos2d::ui::ScrollView::create();
    addChild(scrollView);
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);

    scrollView->setBackGroundColorType(Layout::BackGroundColorType::NONE);
    scrollView->setInertiaScrollEnabled(true);
    scrollView->setContentSize(cocos2d::Size(r.size.width, r.size.height));
    scrollView->setAnchorPoint(Vec2(0,1));
    scrollView->setPosition(Vec2(0,r.size.height));
    main->addChild(this);
    InitPanel();
}

bool PanelBase::HideShow(bool hide, PanelBase *substitute)
{
    MoveTo *action=NULL;
    CallFunc *callback=nullptr;
    if (NULL!=substitute)
        callback = CallFunc::create([hide, substitute](){
            substitute->HideShow(!hide);
        });
    visible=false;
    if (hide)
        action = MoveTo::create(0.1f, cocos2d::Vec2(getContentSize().width*(PROPERTIES_PANEL_TONGUE_PERCENTAGE-1.0), getPositionY()));
    else if (NULL==substitute)
    {
        action = MoveTo::create(0.1f, cocos2d::Vec2(0, getPositionY()));
        visible=true;
    }
    
    if (NULL==action)
    {
        //Substitution case when panel to substitude is closed
        if (NULL!=substitute)
            return substitute->HideShow(hide);
    }
    
    if (nullptr!=callback){
        //Substitution case when panel to substitude is opened
        auto seq = Sequence::create(action, callback, NULL);
        runAction(seq);
    }
    else
        //No Substitution
        runAction(action);
    OnHideShow();
    return visible;
}

void PanelBase::CollapseAllSubpanelsButThis(SubpanelBase *subPanel)
{
    Vector<Node*> _childrens = scrollView->getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        SubpanelBase *sub=dynamic_cast<SubpanelBase*>(*it);
        if (sub==NULL) continue;
        if (sub==subPanel) continue;
        if (sub->IsCollapsed()) continue;
        sub->SetCollapsed(true);
    }
}

void SubpanelBase::DropDownMenuCallbackSubPanel::OnSizeChanged(float oldSize, float newSize)
{
//    parent->GetParent()->InitLayout();
    parent->UpdateLayout();
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

void SubpanelBase::CalculateHeight()
{
    int height=0;
    if (collapsed)
        height=SUBPANEL_ITEM_HEIGHT;
    else
    {
        Vector<Node*> _childrens = getChildren();
        float lastChildrenYCoord=-1;
        for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
        {
            if (!(*it)->isVisible() || (*it)->getPositionY()==lastChildrenYCoord) continue;
            height+=(*it)->getContentSize().height;
            lastChildrenYCoord=(*it)->getPositionY();
        }
        height+=2*GetYPadding();
    }
    Resize(height);
}

void SubpanelBase::HideElement(Node *n, bool hide)
{
    if (n)
        n->setVisible(!hide);
}

void SubpanelBase::EnableElement(Widget *w, bool enable)
{
    w->setEnabled(enable);
    Color3B c=enable?Colors::Instance()->GetUIColor(Colors::UIColorsId::WidgetBackGround):Color3B::GRAY;
    if (dynamic_cast<DropDownMenu*>(w)!=NULL)
        dynamic_cast<DropDownMenu*>(w)->setBackGroundColor(c);
}

void SubpanelBase::UpdateLayout()
{
    CalculateHeight();
    PositionElements();
    GetParent()->InitLayout();
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
   // auto modifyHeight = ActionTween::create(0.1, "height", getContentSize().height, newHeight);
//    runAction(modifyHeight);
    cocos2d::Size s=cocos2d::Size(getContentSize().width, newHeight);
    setContentSize(s);
    PositionElements();
    parent->InitLayout();
}

void SubpanelBase::updateTweenAction(float value, const std::string& key)
{
    if ("height"==key){
        cocos2d::Size s=cocos2d::Size(getContentSize().width, value);
        setContentSize(s);
        PositionElements();
        parent->InitLayout();
    }
    
}

//void SubpanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{
////    Color3B cc=Colors::Instance()->GetUIColor(Colors::SubPanel);
////    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F(((float)cc.r)/255.f,((float)cc.g)/255.f,((float)cc.b)/255.f,1.0));
//    if (flags&FLAGS_CONTENT_SIZE_DIRTY)
//    {
////        PositionElements();
//        // parent->InitLayout();
//        
//    }
//}

void SubpanelBase::InitWithContent(PanelBase *_parent, cocos2d::Size s)
{
    parent.reset(_parent);
    setClippingEnabled(true);
    collapsed=true;
    dropDownCallback.reset(new DropDownMenuCallbackSubPanel(this));
    setContentSize(s);
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
    setAnchorPoint(Vec2(0,1));
    CreateControls();
    PositionElements();
    CheckShowElements();

    setBackGroundImage("leftMenuPanel.png");
    cocos2d::Rect rr(20, 20, getBackGroundImageTextureSize().width-40, getBackGroundImageTextureSize().height-40);
    setBackGroundImageScale9Enabled(true);
    setBackGroundImageCapInsets(rr);
}

void SubpanelBase::TouchEventCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Node* node = dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (node->getTag()==PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW)
            {
                collapsed=!collapsed;
                if(!collapsed)
                    parent->CollapseAllSubpanelsButThis(this);
                CheckShowElements();
            }
            else
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

void SubpanelBase::SetCollapsed(bool c)
{
    collapsed=c;
    CheckShowElements();
}