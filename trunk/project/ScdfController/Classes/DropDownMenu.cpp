//
//  DropDownMenu.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 29/12/14.
//
//

#include "DropDownMenu.h"

using namespace SCDFC;
using namespace cocos2d;
using namespace ui;

#define DROPDOWNN_BACK_COLOR Color3B(120,120,120)

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

DropDownMenu *DropDownMenu::CreateMenu(cocos2d::Size s)
{
    DropDownMenu *menu=DropDownMenu::create();
    menu->setContentSize(s);
    menu->ScrollView::setDirection(Direction::NONE);
    menu->addEventListener(CC_CALLBACK_2(DropDownMenu::OnControlTouch, menu));
    menu->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    menu->setAnchorPoint(Vec2(0,1));
    menu->setBackGroundColorType(BackGroundColorType::NONE);
    return menu;
}

void DropDownMenu::ScrollToSelected()
{
    Node *item=getItem(_curSelectedIndex);
    if(NULL==item) return;
    float elementsHeight=item->getContentSize().height;
    getInnerContainer()->setPosition(Vec2(0,-getInnerContainerSize().height+getContentSize().height+((_curSelectedIndex)*elementsHeight)));
}

DropDownMenu::~DropDownMenu()
{
    removeAllItems();
}


void DropDownMenu::InitData(std::vector<std::string> data, float itemHeight)
{
    if (itemHeight!=getContentSize().height)
        ResizeAndScroll(itemHeight, true);
    removeAllItems();
    
    if (0==data.size())
    {
        Text *model = Text::create("No data","Arial",20);
        model->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height));
        model->ignoreContentAdaptWithSize(false);
        model->setTextVerticalAlignment(TextVAlignment::CENTER);
        model->setTextHorizontalAlignment(TextHAlignment::CENTER);
        model->setColor(Color3B::WHITE);
        setInnerContainerSize(cocos2d::Size(model->getContentSize()));
        pushBackCustomItem(model);
    }
    else{
        for (int i=0; i<data.size(); i++)
        {
            Text *model = Text::create(data[i],"Arial",20);
            model->setTouchEnabled(true);
            model->setContentSize(cocos2d::Size(getContentSize().width-5,getContentSize().height));
            model->ignoreContentAdaptWithSize(false);
            model->setTextVerticalAlignment(TextVAlignment::CENTER);
            model->setTextHorizontalAlignment(TextHAlignment::CENTER);
            model->setColor(Color3B::WHITE);
            pushBackCustomItem(model);
        } 
        setInnerContainerSize(cocos2d::Size(getContentSize().width,data.size()*getContentSize().height));
    }
    SetSelectedIndex(0);
}