
#include "SCDFCDefinitions.h"
#include "AppDelegate.h"
#include "MainScene.h"
#include "ControlUnit.h"
#include "SCDFCItems.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "ControlUnit.h"
#include "PropertiesPanel.h"
#include <platform/CCDevice.h>
#include "Logging.h"
#include "ScdfSensorAPI.h"

using namespace SCDFC;
USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);


    cocos2d::Size designSize = director->getOpenGLView()->getDesignResolutionSize();
    LOGD("Design resolution size: %f %f\n",designSize.width,designSize.height);
    Vec2 vOrigin = director->getVisibleOrigin();
    LOGD("Visible origin: %f %f\n",vOrigin.x,vOrigin.y);
    cocos2d::Size vSize = director->getVisibleSize();
    LOGD("Visible size: %f %f\n",vSize.width,vSize.height);
    cocos2d::Size frameSize = director->getOpenGLView()->getFrameSize();
    LOGD("FRAME size: %f %f\n",frameSize.width,frameSize.height);
    //int orientation = director->getOpenGLView()->getDeviceOrientation();
    //LOGD("Orientation: %d",orientation);
    //float zoomfactor = director->getOpenGLView()->getFrameZoomFactor();
    //LOGD("Zoom factor: %f",zoomfactor);
    //int retinafactor = director->getOpenGLView()->getRetinaFactor();
    //LOGD("Retina factor: %d",retinafactor);
    float scaleX = director->getOpenGLView()->getScaleX();
    float scaleY = director->getOpenGLView()->getScaleY();
    LOGD("Scale X: %f - Y: %f\n",scaleX,scaleY);
    cocos2d::Rect vRect = director->getOpenGLView()->getVisibleRect();
    LOGD("Visible RECT X:%f Y:%f W:%f H:%f\n",vRect.origin.x, vRect.origin.y, vRect.size.width,vRect.size.width);

    LOGD("DPI : %d\n", Device::getDPI() );

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    float scaleFactor=((float)Device::getDPI())/132.0;
    cocos2d::Size s=director->getOpenGLView()->getFrameSize();
    director->getOpenGLView()->setDesignResolutionSize((int)ceil(s.width/scaleFactor), (int)ceil(s.height/scaleFactor), ResolutionPolicy::SHOW_ALL);
    
    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();

    // run
    director->runWithScene(scene);
    
    scdf::theSensorAPI()->InitFramework();

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}