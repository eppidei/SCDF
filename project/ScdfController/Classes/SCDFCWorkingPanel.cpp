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
#include "MultiSender.h"
#include "Logging.h"
#include "LoadSavePanel.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

WorkingPanel *WorkingPanel::CreateCustomPanel(MainScene *main, cocos2d::Rect r)
{
    WorkingPanel *panel=(WorkingPanel*)WorkingPanel::create();
    panel->InitWithContent(main,r);
    return panel;
}

void WorkingPanel::InitWithContent(MainScene *main, cocos2d::Rect r)
{
    parent=main;
    active=true;
    collisionDetected=false;
    draggingRect=cocos2d::Rect::ZERO;
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    parent->addChild(this,-2);
    cocos2d::Rect rr(0, 0, r.size.width, r.size.height);
    auto backGroundImage = Sprite::create("Background.png");
    cocos2d::Rect rrr(0, 0, backGroundImage->getTexture()->getContentSizeInPixels().width, backGroundImage->getTexture()->getContentSizeInPixels().height);
    backGroundImage->setAnchorPoint(Vec2(0,1));
    backGroundImage->setPosition(0,r.size.height);
    backGroundImage->setScale(r.size.width/rrr.size.width, r.size.height/rrr.size.height);
    backGroundImage->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
    addChild(backGroundImage,-3);

    patch.reset(new ControlUnitPatch());
}

void WorkingPanel::SetDraggingRect(cocos2d::Rect _draggingRect)
{
    draggingRect=_draggingRect;
}

void WorkingPanel::CheckAddControl(int buttonTag)
{
    if (0==draggingRect.size.width || collisionDetected)
    {
        collisionDetected=false;
        return;
    }
    ItemBase* item = ItemBase::CreateItem(draggingRect, buttonTag);
    // we use unique ptr just to let Cereal easily serialize
    // the patch:

    // transfer ownership of control unit to the patch:
    std::unique_ptr<ControlUnit> unitUnique(item->GetControlUnit());
    patch->units.push_back(std::move(unitUnique));

    addChild(item,10);
    parent->AttachItem(item);

}

void WorkingPanel::CheckRemoveControl(Node *n)
{
    if (n->getPositionY()<=getPosition().y && (n->getPositionX()>=getPosition().x)
        && (n->getPositionX()+n->getContentSize().width)<=getContentSize().width
        && (n->getPositionY()-n->getContentSize().height)>=getPosition().y-getContentSize().height)
        return;

    for (int i=0;i<patch->units.size();++i)
    {
        if (patch->units[i]->GetItem()==n)
        {
        	parent->DetachItem((ItemBase*)n);
            removeChild(n); // does this delete n? if not, we have a leak
            patch->units.erase(patch->units.begin()+i); // this deletes the unit!
            printf("Control removed from working space\n");
            return;
            // remove "return" if you ever plan to put an item in the patch twice
            // but consider that the index will break after the first deletion
        }
    }
    //parent->DetachItem((ItemBase*)n);
    //removeChild(n);
    //LOGD("Control removed from working space\n");
}

void TestSerialization();

void WorkingPanel::ToggleActiveState()
{
    active=!active;
    TestSerialization();
}

void WorkingPanel::DrawGrid()
{
    DrawPrimitives::setDrawColor4F(0.1, 0.1, 0.1, 1.0);
    
    for(int i=0;i<getContentSize().width;i+=parent->GetGridDistance())
        DrawPrimitives::drawLine(Vec2(i,getContentSize().height), Vec2(i,0));
    for(int i=0;i<getContentSize().height;i+=parent->GetGridDistance())
        DrawPrimitives::drawLine(Vec2(0,i), Vec2(getContentSize().width,i));
}

void WorkingPanel::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
    if (!active)
        DrawGrid();
    
    if (0!=draggingRect.size.width)
    {
        Color4F color(0.1,0.1,0.1, 1.0);
        if (collisionDetected)
            color=Color4F(1.0,0,0, 1.0);
        
        DrawPrimitives::drawSolidRect(draggingRect.origin, Vec2(draggingRect.origin.x+draggingRect.size.width,draggingRect.
            origin.y-draggingRect.size.height),color);
    }
}

void WorkingPanel::DetectCollisions(cocos2d::Rect r)
{
    DoDetectCollisions(NULL, r, NULL);
}

void WorkingPanel::DetectCollisions(Node *item)
{
    cocos2d::Rect rItem(item->getPositionX(), item->getPositionY(), item->getContentSize().width, item->getContentSize().height);
    DoDetectCollisions(item, rItem, NULL);
}

bool RectIntersection(cocos2d::Rect r1, cocos2d::Rect r2)
{
    return !(r1.origin.x > r2.origin.x+r2.size.width  ||
             r2.origin.x > r1.origin.x+r1.size.width  ||
             r1.origin.y < r2.origin.y-r2.size.height ||
             r2.origin.y < r1.origin.y-r1.size.height);
}
void WorkingPanel::DoDetectCollisions(Node *item, cocos2d::Rect r, bool *collision)
{
    bool *c=collision!=NULL?collision:&collisionDetected;
    *c=false;
    for (int i=0;i<patch->units.size();++i)
    {
    	Layout* testItem = dynamic_cast<Layout*>(patch->units[i]->GetItem());
        if (NULL!=item && item==testItem) continue;
        cocos2d::Rect rItem(testItem->getPositionX(), testItem->getPositionY(), testItem->getContentSize().width, testItem->getContentSize().height);
        if(RectIntersection(r,rItem))
        {
            *c=true;
            return;
        }
    }
}

bool WorkingPanel::OnControlMove(Ref *pSender, Vec2 touchPos, cocos2d::ui::Widget::TouchEventType type)
{
    if (active) return false;
    Node* item = dynamic_cast<Node*>(pSender);
    static Vec2 dragStartPoint;
    static Vec2 touchStartPos;
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
        {
            parent->EnableScrollView(false);
            touchStartPos=touchPos;
            dragStartPoint=item->getPosition();
            break;
        }
        case Widget::TouchEventType::MOVED:
        {
            float diff_x=touchPos.x-touchStartPos.x;
            float diff_y=touchPos.y-touchStartPos.y;
            float newX=dragStartPoint.x+diff_x;
            float newY=dragStartPoint.y+diff_y;
            cocos2d::Rect r(newX, newY, item->getContentSize().width,item->getContentSize().height);
            parent->SnapToGrid(r);
            item->setPosition(r.origin);

            DetectCollisions(item);
            int opacity=255;
            if (collisionDetected)
                opacity=80;
            item->setOpacity(opacity);
        }
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
        {
            parent->EnableScrollView(true);
            if (collisionDetected)
            {
                item->setPosition(dragStartPoint);
                item->setOpacity(255);
                collisionDetected=false;
            }
            CheckRemoveControl(item);
        }
            break;
        default:
            break;
    }
    return true;
}
