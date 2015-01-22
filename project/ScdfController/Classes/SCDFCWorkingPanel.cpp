//
//  SCDFCWorkingPanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//
#include "SCDFCDefinitions.h"

#include "MainScene.h"
#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "SCDFCItems.h"
#include "PropertiesPanel.h"
#include "ControlUnit.h"

using namespace SCDFC;
using namespace cocos2d;
using namespace ui;

WorkingPanel *WorkingPanel::CreateCustomPanel(MainScene *main, Rect r)
{
    WorkingPanel *panel=(WorkingPanel*)WorkingPanel::create();
    panel->InitWithContent(main,r);
    return panel;
}

void WorkingPanel::InitWithContent(MainScene *main, Rect r)
{
    parent=main;
    active=true;
    draggingRect=Rect::ZERO;
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    parent->addChild(this);
}

void WorkingPanel::SetDraggingRect(cocos2d::Rect _draggingRect)
{
    draggingRect=_draggingRect;
}

void WorkingPanel::CheckAddControl(int buttonTag)
{
    if (0==draggingRect.size.width) return;
    
    auto item = ItemBase::CreateItem(draggingRect, buttonTag);
    addChild(item);
    //item->addTouchEventListener(CC_CALLBACK_2(WorkingPanel::OnControlTouch, this));
    parent->AttachItem(item);
    items.push_back(item);
}

void WorkingPanel::CheckRemoveControl(Node *n)
{
    if (n->getPositionY()<=getPosition().y && (n->getPositionX()>=getPosition().x)
        && (n->getPositionX()+n->getContentSize().width)<=getContentSize().width
        && (n->getPositionY()-n->getContentSize().height)>=getPosition().y-getContentSize().height)
        return;
    for (int i=0;i<items.size();++i){
        if (items[i]==n) items.erase(items.begin()+i);
    }
    parent->DetachItem((ItemBase*)n);
    removeChild(n);
    printf("Control removed from working space\n");
}

void WorkingPanel::ToggleActiveState()
{
    active=!active;
//    for (auto it=items.begin();it!=items.end();++it)
//    {
//        if (!active)
//            it->setOpacity(80);
//        else
//            it->setOpacity(255);
//    }
    
}

void WorkingPanel::DrawGrid()
{
    Color4F c(0.9,0.9,0.9,1);
    if (!active)
        c=Color4F(0.6,0.6,0.6,1);
    DrawPrimitives::drawSolidRect(Vec2(0,0), Vec2(getContentSize().width,getContentSize().height), c);
    DrawPrimitives::setDrawColor4F(0.3, 0.3, 0.3, 0.6);
    
    for(int i=0;i<getContentSize().width;i+=parent->GetGridDistance())
        DrawPrimitives::drawLine(Vec2(i,getContentSize().height), Vec2(i,0));
    for(int i=0;i<getContentSize().height;i+=parent->GetGridDistance())
        DrawPrimitives::drawLine(Vec2(0,i), Vec2(getContentSize().width,i));
    
}

void WorkingPanel::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
    DrawGrid();
    if (0!=draggingRect.size.width)
        DrawPrimitives::drawSolidRect(draggingRect.origin, Vec2(draggingRect.origin.x+draggingRect.size.width,draggingRect.origin.y-draggingRect.size.height),Color4F(1.0f,20.0f/255.0f,20.0f/255.0f, 0.2f));
    Layout::visit(renderer, parentTransform, parentFlags);
}

//void WorkingPanel::OnControlTouch(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
//{
////    if (!active)
////    {
////        OnControlMove(pSender, type);
////        return;
////    }
//    ui::Button* button = dynamic_cast<ui::Button*>(pSender);
//    static Vec2 dragStartPoint;
//    switch (type)
//    {
//        case Widget::TouchEventType::BEGAN:
//        {
////            ListView* lv = ListView::create();
////            Button* model = Button::create();
////            model->setTouchEnabled(true);
////            model->setScale9Enabled(true);
////            model->setContentSize(Size(100, 40));
////            model->ignoreContentAdaptWithSize(true);
////            model->setTitleText("Text Button");
////            model->setTitleFontSize(20);
////            model->setTitleColor(Color3B(255,0,0));
////            model->setPressedActionEnabled(true);
////            lv->setItemModel(model);
////            
////            for (int i=0; i<5; i++)
////            {
////                lv->pushBackDefaultItem();
////            }
////            addChild(lv);
////            lv->setItemsMargin(5);
////            lv->setGravity(ListView::Gravity::CENTER_VERTICAL);
////            lv->setSize(Size(100,100));
////            lv->setBackGroundColorType(LAYOUT_COLOR_SOLID);
////            lv->setBackGroundColor(Color3B::GREEN);
////            lv->setPosition(button->getPosition());
////            lv->addEventListener(CC_CALLBACK_2(WorkingPanel::selectedItemEvent, this));
//            break;
//        }
//        case Widget::TouchEventType::MOVED:
//        {
//        }
//            break;
//        case Widget::TouchEventType::ENDED:
//            break;
//        case Widget::TouchEventType::CANCELED:
//            break;
//        default:
//            break;
//    }
//}

//void WorkingPanel::selectedItemEvent(Ref *pSender, ListView::EventType type)
//{
//    switch (type)
//    {
//        case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_START:
//        {
//            ListView* listView = static_cast<ListView*>(pSender);
//            CC_UNUSED_PARAM(listView);
//            CCLOG("select child start index = %ld", listView->getCurSelectedIndex());
//            ((Button*)listView->getItem(listView->getCurSelectedIndex()))->setTitleColor(Color3B(0,0,255));
//            break;
//        }
//        case cocos2d::ui::ListView::EventType::ON_SELECTED_ITEM_END:
//        {
//            ListView* listView = static_cast<ListView*>(pSender);
//            CC_UNUSED_PARAM(listView);
//            CCLOG("select child end index = %ld", listView->getCurSelectedIndex());
//            ((Button*)listView->getItem(listView->getCurSelectedIndex()))->setTitleColor(Color3B(255,0,0));
//            break;
//        }
//        default:
//            break;
//    }
//}

bool WorkingPanel::OnControlMove(Ref *pSender, Vec2 touchPos, cocos2d::ui::Widget::TouchEventType type)
{
    if (active) return false;
    Widget* button = dynamic_cast<Widget*>(pSender);
    static Vec2 dragStartPoint;
    static Vec2 touchStartPos;
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
        {
            parent->EnableScrollView(false);
            touchStartPos=touchPos;
            dragStartPoint=button->getPosition();
            break;
        }
        case Widget::TouchEventType::MOVED:
        {
            float diff_x=touchPos.x-touchStartPos.x;
            float diff_y=touchPos.y-touchStartPos.y;
            float newX=dragStartPoint.x+diff_x;
            float newY=dragStartPoint.y+diff_y;
            Rect r(newX, newY, button->getContentSize().width,button->getContentSize().height);
            parent->SnapToGrid(r);
            button->setPosition(r.origin);
        }
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
        {
            parent->EnableScrollView(true);
            auto node = dynamic_cast<Node*>(pSender);
            CheckRemoveControl(node);
        }
            break;
        default:
            break;
    }
    return true;
}