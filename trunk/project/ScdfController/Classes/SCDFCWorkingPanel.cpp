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
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "SCDFCItems.h"
#include "PropertiesPanel.h"
#include "MultiSender.h"
#include "Logging.h"

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
    setTouchEnabled(true);
    addTouchEventListener(CC_CALLBACK_2(WorkingPanel::PanelTouchEvent, this));
    cocos2d::Rect rr(0, 0, r.size.width, r.size.height);
    auto backGroundImage = Sprite::create("background.png");
    cocos2d::Rect rrr(0, 0, backGroundImage->getTexture()->getContentSizeInPixels().width, backGroundImage->getTexture()->getContentSizeInPixels().height);
    backGroundImage->setAnchorPoint(Vec2(0,1));
    backGroundImage->setPosition(0,r.size.height);
    backGroundImage->setScale(r.size.width/rrr.size.width, r.size.height/rrr.size.height);
    backGroundImage->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
    addChild(backGroundImage,-3);

    patch.reset(new ControlUnitPatch(this));
}

void WorkingPanel::SetDraggingRect(cocos2d::Rect _draggingRect)
{
    draggingRect=_draggingRect;
}

template <class ItemType> void WorkingPanel::CheckAddControl()
{
    cocos2d::Rect rr=draggingRect;
    rr.origin=convertToWorldSpace(rr.origin);
	if (!IsInsideVisibleSpace(rr) /*0==draggingRect.size.width*/ || collisionDetected)
    {
        collisionDetected=false;
        return;
    }
    LOGD("Add control");
    ItemBase* item = ItemBase::CreateItem<ItemType>();
    item->setPosition(draggingRect.origin);
    // we use unique ptr just to let Cereal easily serialize
    // the patch:

    // transfer ownership of control unit to the patch:
    //std::unique_ptr<ControlUnit> unitUnique(item->GetControlUnit());
    //patch->items.push_back(std::move(unitUnique));
    patch->items.push_back(item);

    addChild(item,10);
    parent->AttachItem(item);

}
void WorkingPanel::NewPatch()
{
    currentPatchName.clear();
    for (int i=0;i<patch->items.size();++i)
    {
        parent->DetachItem(patch->items[i]);
        removeChild(patch->items[i]);
    }
    patch->items.clear();
}

void WorkingPanel::RemoveControl(Node *n)
{
    if (NULL==n) return;
    for (int i=0;i<patch->items.size();++i)
    {
        if (patch->items[i]==n)
        {
        	parent->DetachItem((ItemBase*)n);
            removeChild(n);
            patch->items.erase(patch->items.begin()+i);
            LOGD("Control removed from working space\n");
            break;
        }
    }
}

void WorkingPanel::OnDiscardPatch()
{
    NewPatch();
}


void WorkingPanel::OnSavePatch(std::string patchName, bool newProject)
{
    currentPatchName=patchName;
	patch->SaveToFile(patchName);
    if (newProject)
        NewPatch();
}

void WorkingPanel::OnLoadPatch(std::string patchName)
{
    currentPatchName=patchName;
	for (int i=0;i<patch->items.size();++i)
	{
		parent->DetachItem(patch->items[i]);
	    removeChild(patch->items[i]);
	}
    patch->items.clear();
	patch->LoadFromFile(patchName);

	for (int i=0;i<patch->items.size();++i)
	{
		addChild(patch->items[i],10);
	    parent->AttachItem(patch->items[i]);
	}
}

void WorkingPanel::SetActive(bool _active)
{
	LOGD("TOGGLE ACTIVE STATE");
    active=_active;
    parent->UpdateEditButton();
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
    
    cocos2d::Rect rr=draggingRect;
    rr.origin=convertToWorldSpace(rr.origin);
    
    Color4F color(0.1,0.1,0.1, 1.0);
    if (collisionDetected || !IsInsideVisibleSpace(rr))
        color=Color4F(1.0,0,0, 1.0);
        
    DrawPrimitives::drawSolidRect(draggingRect.origin, Vec2(draggingRect.origin.x+draggingRect.size.width,draggingRect.
        origin.y-draggingRect.size.height),color);
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
    for (int i=0;i<patch->items.size();++i)
    {
        Layout* testItem = dynamic_cast<Layout*>(patch->items[i]);
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
    ItemBase* item = dynamic_cast<ItemBase*>(pSender);
    static Vec2 dragStartPoint;
    static Vec2 touchStartPos;
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
        {
            item->NotifyEvent(SCDFC_EVENTS_Select_Item);
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
            r.origin=convertToWorldSpace(r.origin);
            if (collisionDetected || !IsInsideVisibleSpace(r))
                opacity=80;
            item->setOpacity(opacity);
        }
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
        {
            parent->EnableScrollView(true);
            Vec2 itemSpaceCoord=convertToWorldSpace(Vec2(item->getPositionX(), item->getPositionY()));
            cocos2d::Rect itemRect={itemSpaceCoord.x, itemSpaceCoord.y, item->getContentSize().width, item->getContentSize().height};
            if (collisionDetected || !IsInsideVisibleSpace(itemRect))
            {
                item->setPosition(dragStartPoint);
                item->setOpacity(255);
                collisionDetected=false;
            }
        }
            break;
        default:
            break;
    }
    return true;
}


void WorkingPanel::OnItemTouchBegan(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!OnControlMove(item, widget->getTouchBeganPosition(), type))
        DoOnItemTouchBegan(item, widget, type);
}
void WorkingPanel::OnItemTouchMoved(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!OnControlMove(item, widget->getTouchMovePosition(), type))
        DoOnItemTouchMoved(item, widget, type);
}

void WorkingPanel::OnItemTouchEnded(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!OnControlMove (item, widget->getTouchEndPosition(), type))
        DoOnItemTouchEnded(item, widget, type);
}

void WorkingPanel::DoOnItemTouchBegan(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!item->OnItemTouchBegan(widget, type)) return;
    int itemGroupId=item->GetGroupID();
    if (item->IsMaster())
    {
        for (int i=0;i<patch->items.size();++i)
        {
            if (patch->items[i]->GetGroupID()!=itemGroupId || patch->items[i]==item) continue;
            if (patch->items[i]->GetID()!=item->GetID()) continue;
            patch->items[i]->OnItemTouchBegan(widget,type);
        }
    }
}
void WorkingPanel::DoOnItemTouchMoved(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!item->OnItemTouchMoved(widget, type)) return;
    int itemGroupId=item->GetGroupID();
    if (item->IsMaster())
    {
        for (int i=0;i<patch->items.size();++i)
        {
            if (patch->items[i]->GetGroupID()!=itemGroupId || patch->items[i]==item) continue;
            if (patch->items[i]->GetID()!=item->GetID()) continue;
            patch->items[i]->OnItemTouchMoved(widget,type);
        }
    }
}

bool WorkingPanel::IsInsideVisibleSpace(cocos2d::Rect rect)
{
    cocos2d::Rect rrr=getBoundingBox();
    float workingPanelY=fmin(getContentSize().height,rrr.size.height+rrr.origin.y);
    float workingHeight=workingPanelY-fmax(0,rrr.origin.y);
    float workingPanelX=fmax(0,rrr.origin.x);
    float workingWidth=workingPanelX+fmin(getContentSize().width,rrr.size.width+rrr.origin.x);
    
    cocos2d::Rect workingSpaceRect(workingPanelX, workingPanelY, workingWidth, workingHeight);
    
    if (rect.origin.y<=workingSpaceRect.origin.y && rect.origin.x>=workingSpaceRect.origin.x
        && (rect.origin.x+rect.size.width)<=workingSpaceRect.size.width
        && (rect.origin.y-rect.size.height)>=(workingSpaceRect.origin.y-workingSpaceRect.size.height))
        return true;

    return false;
}

void WorkingPanel::DoOnItemTouchEnded(ItemBase *item, Widget* widget, cocos2d::ui::Widget::TouchEventType type)
{
    if (!item->OnItemTouchEnded(widget, type)) return;
    int itemGroupId=item->GetGroupID();
    if (item->IsMaster())
    {
        for (int i=0;i<patch->items.size();++i)
        {
            if (patch->items[i]->GetGroupID()!=itemGroupId || patch->items[i]==item) continue;
            if (patch->items[i]->GetID()!=item->GetID()) continue;
            patch->items[i]->OnItemTouchEnded(widget,type);
        }
    }
}

void WorkingPanel::PanelTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    static Vec2 dragStartUpdated;
    if (active) return;
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            dragStartUpdated=getTouchBeganPosition();
            break;
        case Widget::TouchEventType::MOVED:
        {
            float diff_x=dragStartUpdated.x-getTouchMovePosition().x;
            float diff_y=dragStartUpdated.y-getTouchMovePosition().y;
            cocos2d::Size s=parent->getContentSize();
            float minX=parent->getContentSize().width/2.0-getContentSize().width;
            float maxY=parent->getContentSize().height/2.0+getContentSize().height;
            float newPosX=fmax(minX, fmin(parent->getContentSize().width/2.0, getPositionX()-diff_x));
            float newPosY=fmin(maxY, fmax(parent->getContentSize().height/2.0, getPositionY()-diff_y));
            setPosition(Vec2(newPosX, newPosY));
            dragStartUpdated=getTouchMovePosition();
        }
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            break;
        default:
            break;
    }
}

void WorkingPanel::Deserialize(SerializableAppData *appdata)
{
    setPositionX(appdata->patch_x);
    setPositionY(appdata->patch_y);
}

template void WorkingPanel::CheckAddControl<ItemSlider>();
template void WorkingPanel::CheckAddControl<ItemPad>();
template void WorkingPanel::CheckAddControl<ItemKnob>();
template void WorkingPanel::CheckAddControl<ItemKeyboard>();
template void WorkingPanel::CheckAddControl<ItemMultipad>();
template void WorkingPanel::CheckAddControl<ItemSwitch>();
template void WorkingPanel::CheckAddControl<ItemWheel>();
