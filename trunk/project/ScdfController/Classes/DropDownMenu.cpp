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
void DropDownMenu::ResizeAndScroll(float newHeight, bool disableScrolling)
{
    parentHeightWithoutMenu=getParent()->getContentSize().height-getContentSize().height;
    auto modifyHeight = ActionTween::create(0.1, "height", getContentSize().height, newHeight);
    auto callback = CallFunc::create([this, disableScrolling](){
        this->ScrollToSelected();
        this->ScrollView::setDirection(disableScrolling?ScrollView::Direction::NONE:ScrollView::Direction::VERTICAL);
    });
    auto seq = Sequence::create(modifyHeight, callback, NULL);
    runAction(seq);
}

void DropDownMenu::ToggleOpenMenu()
{
    if (getItems().size()<=1) return;
    float itemHeight=getItem(0)->getContentSize().height;
    bool disableScrolling;
    float newHeight=getContentSize().height;
    if (opened)
    {
        newHeight-=fmin(itemHeight*(getItems().size()-1),MAX_OPENED_MENU_HEIGHT);
        disableScrolling=true;
    }
    else
    {
        newHeight+=fmin(itemHeight*(getItems().size()-1),MAX_OPENED_MENU_HEIGHT);
        disableScrolling=false;
    }
    opened=!opened;
    ResizeAndScroll(newHeight,disableScrolling);
}

//void DropDownMenu::SetSize(const Size & size)
//{
//    closedHeight=size.height;
//    setContentSize(size);
//}

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
//    if (_curSelectedIndex>=itemsData.size()){
//        printf("\nError retrieving item info\n");
//        return DropDownMenuData("",0);
//    }
//    return itemsData[_curSelectedIndex];
    return _curSelectedIndex;
    
}

void DropDownMenu::SetSelectedIndex(int selected)
{
    if (lastSelectedIndex==selected) return;
    _curSelectedIndex=lastSelectedIndex=selected;
    ScrollToSelected();
    if (callback)
        callback->OnSelectItem(this);
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
            ToggleOpenMenu();
            SetSelectedIndex(getCurSelectedIndex());
            break;
        default:
            break;
    }
}

DropDownMenu::DropDownMenu()
{

    opened=false;
    callback=NULL;
    resizeParent=true;
    lastSelectedIndex=-1;
}

void DropDownMenu::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::setDrawColor4B(255, 255, 255, 255);
    DrawPrimitives::drawRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0));
}

template  DropDownMenu *DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size s);
template  DropDownMenu *DropDownMenu::CreateMenu<DropDownColorMenu>(cocos2d::Size s);

template <class DropDownType> DropDownMenu *DropDownMenu::CreateMenu(cocos2d::Size s)
{
    DropDownMenu *menu=DropDownType::create();
    menu->setContentSize(s);
    menu->ScrollView::setDirection(Direction::VERTICAL);
    menu->addEventListener(CC_CALLBACK_2(DropDownMenu::OnControlTouch, menu));
    menu->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    menu->setAnchorPoint(Vec2(0,1));
    menu->setBackGroundColorType(BackGroundColorType::SOLID);
    menu->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::Main_Background));
    return menu;
}

void DropDownMenu::ScrollToSelected()
{
    Node *item=getItem(_curSelectedIndex);
    if(NULL==item) return;
    float elementsHeight=item->getContentSize().height;
    getInnerContainer()->setPosition(Vec2(0,-getInnerContainerSize().height+getContentSize().height+(-(getItemsMargin()/2)+(_curSelectedIndex)*(elementsHeight+getItemsMargin()))));
}

DropDownMenu::~DropDownMenu()
{
    removeAllItems();
}

void DropDownMenu::BeforeInitData(float itemHeight)
{
    if (itemHeight!=getContentSize().height)
        ResizeAndScroll(itemHeight, true);
    removeAllItems();
}

void DropDownMenu::AfterInitData(int numElements)
{
    updateInnerContainerSize();
    SetSelectedIndex(0);
    ScrollView::setDirection(Direction::NONE);
}

bool DropDownMenu::CheckDataSize(std::vector<DropDownMenuData> data)
{
    if (0==data.size())
    {
        Text *model = Text::create("No data","Arial",20);
        model->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height));
        model->ignoreContentAdaptWithSize(false);
        model->setTextVerticalAlignment(TextVAlignment::CENTER);
        model->setTextHorizontalAlignment(TextHAlignment::CENTER);
        model->setColor(Color3B::WHITE);
        pushBackCustomItem(model);
        return false;
    }
    return true;
}

void DropDownMenu::DoInitData(std::vector<DropDownMenuData> data)
{
    for (int i=0; i<data.size(); i++)
    {
        Text *model = Text::create(data[i].text,"Arial",20);
        model->setTouchEnabled(true);
        model->setContentSize(cocos2d::Size(getContentSize().width-6,getContentSize().height));
        model->ignoreContentAdaptWithSize(false);
        model->setTextVerticalAlignment(TextVAlignment::CENTER);
        model->setTextHorizontalAlignment(TextHAlignment::CENTER);
        model->setColor(data[i].c);
        pushBackCustomItem(model);
    }
}

void DropDownColorMenu::DoInitData(std::vector<DropDownMenuData> data)
{
    setItemsMargin(getContentSize().height/2);
            setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    for (int i=0; i<data.size(); i++)
    {
        Layout *model = Layout::create();
        model->setTouchEnabled(true);
        model->setContentSize(cocos2d::Size(getContentSize().width-6,getContentSize().height/2));
        //model->setPosition(Vec2(3,getContentSize().height-getItemsMargin()-(model->getContentSize().height+getItemsMargin())));
        model->ignoreContentAdaptWithSize(false);
        model->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
        model->setBackGroundColor(data[i].c);
        pushBackCustomItem(model);
    }
}

void DropDownMenu::InitData(std::vector<DropDownMenuData> data, float itemHeight)
{
//    itemsData=data;
    BeforeInitData(itemHeight);
    int numElements=1;
    if (CheckDataSize(data))
    {
        DoInitData(data);
        numElements=data.size();
    }
    AfterInitData(numElements);
}
