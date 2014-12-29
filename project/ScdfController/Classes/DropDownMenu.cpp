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

#define OPENED_MENU_HEIGHT 200
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
    bool disableScrolling;
    float newHeight=getContentSize().height;
    if (opened)
    {
        newHeight-=OPENED_MENU_HEIGHT;
        disableScrolling=true;
    }
    else
    {
        newHeight+=OPENED_MENU_HEIGHT;
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

void DropDownMenu::SetMenuPosition(Vec2 pos)
{
    setPosition(pos);
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
        setContentSize(Size(getContentSize().width, value));
        getParent()->setContentSize(Size(getParent()->getContentSize().width,parentHeightWithoutMenu+value));
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
            break;
        default:
            break;
    }
    SetSelectedIndex(getCurSelectedIndex());
}

DropDownMenu::DropDownMenu()
{
    opened=false;
    callback=NULL;
    resizeParent=true;
    showLabel=true;
    lastSelectedIndex=-1;
}

DropDownMenu *DropDownMenu::CreateMenu(Size s)
{
    DropDownMenu *menu=DropDownMenu::create();
    menu->setContentSize(s);
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


void DropDownMenu::InitData(std::vector<std::string> data)
{
    removeAllItems();
    addEventListener(CC_CALLBACK_2(DropDownMenu::OnControlTouch, this));
    if (0==data.size())
    {
        Text *model = Text::create("No data","Arial",20);
        model->setContentSize(Size(getContentSize().width,getContentSize().height));
        model->ignoreContentAdaptWithSize(false);
        model->setTextVerticalAlignment(TextVAlignment::CENTER);
        model->setTextHorizontalAlignment(TextHAlignment::LEFT);
        model->setColor(Color3B::BLACK);
        setInnerContainerSize(Size(model->getContentSize()));
        pushBackCustomItem(model);
    }
    else{
        for (int i=0; i<data.size(); i++)
        {
//            std::ostringstream os;
//            os<<"Pippo "<<i;
            Text *model = Text::create(data[i]/*os.str()*/,"Arial",20);
            model->setTouchEnabled(true);
            model->setContentSize(Size(getContentSize().width,getContentSize().height));
            model->ignoreContentAdaptWithSize(false);
            model->setTextVerticalAlignment(TextVAlignment::CENTER);
            model->setTextHorizontalAlignment(TextHAlignment::LEFT);
            model->setColor(Color3B::BLACK);
            pushBackCustomItem(model);
        } 
        setInnerContainerSize(Size(getContentSize().width,data.size()*getContentSize().height));
    }
    setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    setAnchorPoint(Vec2(0,1));
    setBackGroundColorType(BackGroundColorType::SOLID);
    setBackGroundColor(DROPDOWNN_BACK_COLOR);
}