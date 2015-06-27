//
//  NanoControls.h
//  test_cocos2d_nanovg
//
//  Created by andrea scuderi on 26/06/15.
//
//

#ifndef __test_cocos2d_nanovg__NanoControls__
#define __test_cocos2d_nanovg__NanoControls__

#include <stdio.h>

#include "cocos2d.h"
#include "ui/UIWidget.h"
#include <functional>

struct NVGcontext;

class NanoControlBaseGlHelper
{
public:
    NanoControlBaseGlHelper();
//    ~NanoControlBaseGlHelper();
    void BeginPaint(cocos2d::Rect r);
    void EndPaint();
    NVGcontext *GetContext() { return nvgContext.get(); }
private:
    std::unique_ptr<NVGcontext, void(*)(NVGcontext*)> nvgContext;
};

template <class T> class NanoControlBase : public cocos2d::ui::Widget
{
public:
    static T *Create(cocos2d::Rect r)
    {
        T *pRet = new T();
        if (pRet && pRet->init(r)){
            pRet->autorelease();
        }else{
            delete pRet;
            pRet=NULL;
        }
        return pRet;
    }
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override
    {
        cocos2d::Rect r(getPositionX(), getPositionY(), getContentSize().width, getContentSize().height);
        glHelper->BeginPaint(getBoundingBox());
        OnDraw();
        glHelper->EndPaint();
    }
    virtual bool onTouchBegan(cocos2d::Touch *t, cocos2d::Event *e) override
    {
        if (!Widget::onTouchBegan(t, e))
            return false;
        lastCursorPoint=t->getLocationInView();
        OnTouchDown();
        return true;
    }
    virtual void onTouchEnded(cocos2d::Touch *t, cocos2d::Event *e) override
    {
        Widget::onTouchEnded(t, e);
        lastCursorPoint=t->getLocationInView();
        OnTouchUp();
    }
    virtual void onTouchMoved(cocos2d::Touch *t, cocos2d::Event *e) override
    {
        Widget::onTouchMoved(t, e);
        lastCursorPoint=t->getLocationInView();
        OnTouchMoving();
    }
    virtual void onTouchCancelled(cocos2d::Touch *t, cocos2d::Event *e) override
    {
        Widget::onTouchCancelled(t, e);
        OnTouchCancelled();
    }

    ~NanoControlBase() { OnDestroy(); }
protected:
    NVGcontext *GetNVGContext() { return glHelper->GetContext(); }
    cocos2d::Vec2 lastCursorPoint;
private:

    std::unique_ptr<NanoControlBaseGlHelper> glHelper;
    
    virtual bool init(cocos2d::Rect r)
    {
        glHelper.reset(new NanoControlBaseGlHelper());
        bool ret=Widget::init();
        setAnchorPoint(cocos2d::Vec2(0,1));
        setPosition(r.origin);
        setContentSize(r.size);
        setTouchEnabled(true);
        lastCursorPoint.x=0;
        lastCursorPoint.y=0;
        OnInit();
        return ret;
    }

    virtual void OnInit() {}
    virtual void OnDraw() = 0;
    virtual void OnTouchDown() = 0;
    virtual void OnTouchUp() = 0;
    virtual void OnTouchMoving() = 0;
    virtual void OnTouchCancelled() {}
    virtual void OnDestroy() {}
};

#endif /* defined(__test_cocos2d_nanovg__NanoControls__) */
