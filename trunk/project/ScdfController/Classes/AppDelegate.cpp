#include "SCDFCDefinitions.h"

#include "AppDelegate.h"
#include "MultiSender.h"
#include "MainScene.h"
#include "SCDFCItems.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"

#include "PropertiesPanel.h"
#include <platform/CCDevice.h>
#include "Logging.h"
#include "ScdfSensorAPI.h"

using namespace ScdfCtrl;
USING_NS_CC;

#ifdef ANDROID

#include "ScdfSensorAPI.h"

extern "C" JNIEXPORT jboolean JNICALL Java_org_cocos2dx_cpp_AppActivity_SetupScdfSensorAPI
	(JNIEnv *e, jclass cl)
{
	//sleep(5);
	scdf::theSensorAPI()->InitFramework();
	return true;
}


#endif


namespace Scdf
{
    void createMidiMananager();
}

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

    //sleep(300);

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
    
    float dpi = Device::getDPI();
    float wInches = vSize.width / dpi;

    LOGD("Width in inches %f",wInches);

    const int maxPhoneInches = 6;
    float phoneFactor = wInches > maxPhoneInches ? 1.0 : 0.667; //IsTablet() ? 1 : 1.5;

    float scaleFactor= phoneFactor * ( ((float)Device::getDPI())/132.0 );
    cocos2d::Size s=director->getOpenGLView()->getFrameSize();

    director->getOpenGLView()->setDesignResolutionSize((int)ceil(s.width/scaleFactor), (int)ceil(s.height/scaleFactor), ResolutionPolicy::SHOW_ALL);
    
    cocos2d::Size vSize2 = director->getVisibleSize();
    LOGD("Visible size after scaling: %f %f\n",vSize2.width,vSize2.height);

    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();

    // run
    director->runWithScene(scene);

#ifdef PLATF_IOS
    scdf::theSensorAPI()->InitFramework();
    Scdf::createMidiMananager();
#endif

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
