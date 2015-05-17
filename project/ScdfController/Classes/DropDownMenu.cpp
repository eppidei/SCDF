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
        this->ScrollView::setDirection(disableScrolling?ScrollView::Direction::NONE:ScrollView::Direction::VERTICAL);
        this->EnableTouchEvents(true);
    });
    auto seq = Sequence::create(modifyHeight, callback, NULL);
    runAction(seq);
}

int DropDownMenu::GetNumItems()
{
    return getItems().size();
}

void DropDownMenu::HideItem(int itemIndex, bool hide)
{
    if (itemIndex>=itemsData.size()) return;
    
    if ((hide && itemsData[itemIndex].hidden) || (!hide && !itemsData[itemIndex].hidden)) return;

    if (hide)
        removeItem(itemIndex);
    else
        insertCustomItem(CreateElement(itemIndex,SUBPANEL_ITEM_HEIGHT), itemIndex);
    
    itemsData[itemIndex].hidden=hide;
    refreshView();
    Resize();
}

void DropDownMenu::Resize()
{
    float itemHeight=getItem(0)->getContentSize().height+getItemsMargin();
    float newHeight=opened ? fmin(itemHeight*GetNumItems(),MAX_OPENED_MENU_HEIGHT) : itemHeight;
    DoResizeAndScroll(newHeight,!opened);
}

void DropDownMenu::ToggleOpenMenu()
{
    if (GetNumItems()<=1) return;
    
    EnableTouchEvents(false);
    opened=!opened;
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
    return _curSelectedIndex;
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

void DropDownMenu::OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type)
{
    
    switch (type)
    {
        case ListView::EventType::ON_SELECTED_ITEM_END:
            if(touchEnabled)
                ToggleOpenMenu();
            if (!opened) //on closure
            {
                SetSelectedIndex(getCurSelectedIndex());
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
    Node *item=getItem(_curSelectedIndex);
    if(NULL==item) return;
    float elementsHeight=item->getContentSize().height+getItemsMargin();
    
    getInnerContainer()->setPosition(Vec2(0,(getContentSize().height-getInnerContainerSize().height-getItemsMargin()/2.0)+(_curSelectedIndex*elementsHeight)));
}

DropDownMenu::~DropDownMenu()
{
    removeAllItems();
}

void DropDownMenu::BeforeInitData(float itemHeight)
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
    ScrollView::setDirection(Direction::NONE);
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

Widget *DropDownMenu::CreateElement(int itemIndex, int itemHeight)
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
        pushBackCustomItem(CreateElement(i,SUBPANEL_ITEM_HEIGHT));
}

Widget *DropDownColorMenu::CreateElement(int dataIndex, int itemHeight)
{
    Layout *model = Layout::create();
    model->setTouchEnabled(true);
    model->setContentSize(cocos2d::Size(getContentSize().width,2*itemHeight/3));
    model->ignoreContentAdaptWithSize(false);
    model->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    model->setBackGroundColor(itemsData[dataIndex].c);
    return model;
}

void DropDownMenu::InitData(std::vector<DropDownMenuData> data, float itemHeight)
{
    itemsData=data;
    BeforeInitData(itemHeight);
    if (0==data.size())
        InitNullData();
    else
        DoInitData();
    AfterInitData();
}
