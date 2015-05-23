//
//  DropDownMenu.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 29/12/14.
//
//

#include "DropDownMenu.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;


#define MAX_OPENED_MENU_HEIGHT 200.0
void DropDownMenu::DoResizeAndScroll(float newHeight, bool disableScrolling)
{
    parentHeightWithoutMenu=getParent()->getContentSize().height-getContentSize().height;
    auto modifyHeight = ActionTween::create(0.1, "height", getContentSize().height, newHeight);
    auto callback = CallFunc::create([this, disableScrolling](){
        this->ScrollToSelected();
        this->EnableTouchEvents(true);
    });
    auto seq = Sequence::create(modifyHeight, callback, NULL);
    runAction(seq);
}

int DropDownMenu::GetNumItems()
{
    return (int)(getItems().size());
}

void DropDownMenu::HideItem(int itemIndex, bool hide)
{
    if (itemIndex>=itemsData.size()) return;
    
    if ((hide && itemsData[itemIndex].hidden) || (!hide && !itemsData[itemIndex].hidden)) return;

    if (hide)
        removeItem(itemIndex);
    else
        insertCustomItem(CreateElement(itemIndex), itemIndex);
    
    itemsData[itemIndex].hidden=hide;
    refreshView();
    Resize();
}

void DropDownMenu::Resize()
{
    float newHeight=opened ? fmin(itemHeight*GetNumItems(),MAX_OPENED_MENU_HEIGHT) : itemHeight;
    DoResizeAndScroll(newHeight,!opened);
}

void DropDownMenu::OnToggleOpenMenu()
{
    for(int i=0;i<GetNumItems();++i)
        getItem(i)->setColor(Colors::Instance()->GetUIColor(Colors::DropDownText));
    if (opened)
        getItem(_curSelectedIndex)->setColor(Color3B::BLACK);
}

void DropDownMenu::ToggleOpenMenu()
{
    if (GetNumItems()<=1) return;
    
    EnableTouchEvents(false);
    opened=!opened;
    OnToggleOpenMenu();
    Resize();
}

void DropDownMenu::SetCallback(DropDownMenuCallback *_callback)
{
    callback=_callback;
}

void DropDownMenu::setPosition(const Vec2 &pos)
{
    Widget::setPosition(pos);
    ScrollToSelected();
}

int DropDownMenu::GetSelectedIndex()
{
    return (int)_curSelectedIndex;
}

void DropDownMenu::SetSelectedIndex(int selected)
{
    if (lastSelectedIndex==selected) return;
    _curSelectedIndex=lastSelectedIndex=selected;
    ScrollToSelected();
}

void DropDownMenu::updateTweenAction(float value, const std::string& key)
{
    if ("height"==key){
        setContentSize(cocos2d::Size(getContentSize().width, value));
        if (!resizeParent) return;
        getParent()->setContentSize(cocos2d::Size(getParent()->getContentSize().width,parentHeightWithoutMenu+value));
        if (this->callback)
            this->callback->OnSizeChanged(0,0);
    }
    
}

void DropDownMenu::handleMoveLogic(Touch *touch)
{
    if (opened)
        ScrollView::handleMoveLogic(touch);
    else
        Layout::onTouchMoved(touch, NULL);
}

void DropDownMenu::handleReleaseLogic(Touch *touch)
{
    if (opened)
        ScrollView::handleReleaseLogic(touch);
    else
        Layout::onTouchEnded(touch, NULL);
}

void DropDownMenu::onTouchMoved(Touch *touch, Event *unusedEvent)
{
    handleMoveLogic(touch);
}

void DropDownMenu::onTouchEnded(Touch *touch, Event *unusedEvent)
{
    handleReleaseLogic(touch);
}
void DropDownMenu::onTouchCancelled(Touch *touch, Event *unusedEvent)
{
    handleReleaseLogic(touch);
}

void DropDownMenu::OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type)
{
    
    switch (type)
    {
        case ListView::EventType::ON_SELECTED_ITEM_END:
            if(touchEnabled)
                ToggleOpenMenu();
            if (!opened) //on closure
            {
                SetSelectedIndex((int)getCurSelectedIndex());
                if (callback )
                    callback->OnSelectItem(this);
            }
            break;
        default:
            break;
    }
}

DropDownMenu::DropDownMenu()
{
    opened=false;
    callback=NULL;
    touchEnabled=true;
    resizeParent=true;
    lastSelectedIndex=-1;
}

template  DropDownMenu *DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size s, DropDownMenuCallback *callback);
template  DropDownMenu *DropDownMenu::CreateMenu<DropDownColorMenu>(cocos2d::Size s, DropDownMenuCallback *callback);

template <class DropDownType> DropDownMenu *DropDownMenu::CreateMenu(cocos2d::Size s, DropDownMenuCallback *_callback)
{
    DropDownMenu *menu=DropDownType::create();
    menu->setContentSize(s);
    menu->ignoreContentAdaptWithSize(false);
    menu->ScrollView::setDirection(Direction::VERTICAL);
    menu->addEventListener(CC_CALLBACK_2(DropDownMenu::OnControlTouch, menu));
    menu->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    menu->setAnchorPoint(Vec2(0,1));
    menu->setBackGroundColorType(BackGroundColorType::SOLID);
    menu->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::WidgetBackGround));
    menu->SetCallback(_callback);
    return menu;
}

void DropDownMenu::ScrollToSelected()
{
    float elementsHeight=itemHeight+getItemsMargin();
    
    getInnerContainer()->setPosition(Vec2(0,(getContentSize().height-getInnerContainerSize().height)+(_curSelectedIndex*elementsHeight)));
}

DropDownMenu::~DropDownMenu()
{
    removeAllItems();
}

void DropDownMenu::BeforeInitData()
{
    if (itemHeight!=getContentSize().height)
        DoResizeAndScroll(itemHeight, true);
    removeAllItems();
}

void DropDownMenu::AfterInitData()
{
    setItemsMargin(GetVerticalMargin());
    SetSelectedIndex(0);
    refreshView();
}

void DropDownMenu::InitNullData()
{
    Text *model = Text::create("No data",Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    model->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height));
    model->ignoreContentAdaptWithSize(false);
    model->setTextVerticalAlignment(TextVAlignment::CENTER);
    model->setTextHorizontalAlignment(TextHAlignment::LEFT);
    model->setColor(Color3B::WHITE);
    pushBackCustomItem(model);
}

Widget *DropDownMenu::CreateElement(int itemIndex)
{
    Text *model = Text::create(itemsData[itemIndex].text,Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    model->setTouchEnabled(true);
    model->setContentSize(cocos2d::Size(getContentSize().width-6,itemHeight));
    model->ignoreContentAdaptWithSize(false);
    model->setTextVerticalAlignment(TextVAlignment::CENTER);
    model->setTextHorizontalAlignment(TextHAlignment::LEFT);
    model->setColor(itemsData[itemIndex].c);
    return model;
}

void DropDownMenu::DoInitData()
{
    for (int i=0; i<itemsData.size(); i++)
        pushBackCustomItem(CreateElement(i));
}

Widget *DropDownColorMenu::CreateElement(int dataIndex)
{
    Layout *model = Layout::create();
    model->setTouchEnabled(true);
    model->setContentSize(cocos2d::Size(getContentSize().width,itemHeight));
    model->ignoreContentAdaptWithSize(false);
    model->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    model->setBackGroundColor(itemsData[dataIndex].c);
    return model;
}

void DropDownMenu::InitData(std::vector<DropDownMenuData> data, float _itemHeight)
{
    itemsData=data;
    itemHeight=_itemHeight;
    BeforeInitData();
    if (0==data.size())
        InitNullData();
    else
        DoInitData();
    AfterInitData();
}
