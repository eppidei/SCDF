#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "demo.h"
#include "perf.h"

class HelloWorld : public cocos2d::Layer
{
    PerfGraph fps;
    NVGcontext* vg;
    DemoData data;
    double prevt;
    cocos2d::Vec2 cursorPoint;
    bool premuto;
    
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*) override;
    virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*) override;
    virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*) override;
    virtual void onTouchCancelled(cocos2d::Touch*, cocos2d::Event*) override;
    
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
};

#endif // __HELLOWORLD_SCENE_H__
