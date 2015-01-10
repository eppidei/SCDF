
#include "SCDFCDefinitions.h"
#include "AppDelegate.h"
#include "MainScene.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "ControlUnit.h"
#include "PropertiesPanel.h"
#include <platform/CCDevice.h>
#include "Logging.h"

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


    Size designSize = director->getOpenGLView()->getDesignResolutionSize();
    LOGD("Design resolution size: %f %f",designSize.width,designSize.height);
    Vec2 vOrigin = director->getVisibleOrigin();
    LOGD("Visible origin: %f %f",vOrigin.x,vOrigin.y);
    Size vSize = director->getVisibleSize();
    LOGD("Visible size: %f %f",vSize.width,vSize.height);
    Size frameSize = director->getOpenGLView()->getFrameSize();
    LOGD("FRAME size: %f %f",frameSize.width,frameSize.height);
    //int orientation = director->getOpenGLView()->getDeviceOrientation();
    //LOGD("Orientation: %d",orientation);
    //float zoomfactor = director->getOpenGLView()->getFrameZoomFactor();
    //LOGD("Zoom factor: %f",zoomfactor);
    //int retinafactor = director->getOpenGLView()->getRetinaFactor();
    //LOGD("Retina factor: %d",retinafactor);
    float scaleX = director->getOpenGLView()->getScaleX();
    float scaleY = director->getOpenGLView()->getScaleY();
    LOGD("Scale X: %f - Y: %f",scaleX,scaleY);
    Rect vRect = director->getOpenGLView()->getVisibleRect();
    LOGD("Visible RECT X:%f Y:%f W:%f H:%f",vRect.origin.x, vRect.origin.y, vRect.size.width,vRect.size.width);

    LOGD("DPI : %d", Device::getDPI() );

    director->getOpenGLView()->setDesignResolutionSize(frameSize.width,frameSize.height,ResolutionPolicy::SHOW_ALL);
    director->setContentScaleFactor(3.0);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();

    // run
    director->runWithScene(scene);

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
