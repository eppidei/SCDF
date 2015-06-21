#include "HelloWorldScene.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "platform/CCFileUtils.h"
#include <string>

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

void HelloWorld::draw(Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    //	glfwSwapInterval(0);
    //
    //	glfwSetTime(0);

    double t, dt;
    int winWidth, winHeight;
    int fbWidth, fbHeight;
    float pxRatio;

    struct timeval tv;
    gettimeofday(&tv,NULL);
    
    t= tv.tv_sec * 1000 + tv.tv_usec / 1000;
    dt = t - prevt;
    prevt = t;
    updateGraph(&fps, dt);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    winWidth=visibleSize.width;
    winHeight=visibleSize.height;
    
    //glfwGetCursorPos(window, &mx, &my);

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &fbWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &fbHeight);

    // Calculate pixel ration for hi-dpi devices.
    pxRatio = (float)fbWidth / (float)winWidth;

    // Update and render
    glViewport(0, 0, fbWidth, fbHeight);
	
    if (premuto)
		glClearColor(0,0,0,0);
	else
        glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

    renderDemo(vg, cursorPoint.x, cursorPoint.y, winWidth,winHeight, t, 0, &data);
    renderGraph(vg, 5,5, &fps);

    nvgEndFrame(vg);

    glEnable(GL_DEPTH_TEST);

//  glfwSwapBuffers(window);
//  glfwPollEvents();
    
    // On window destroyed
//    freeDemoData(vg, &data);
//
//    nvgDeleteGLES2(vg);
}
// on "init" you need to initialize your instance

int loadDemoData(NVGcontext* vg, DemoData* data)
{
    int i;
    
    if (vg == NULL)
        return -1;
    
    for (i = 0; i < 12; i++) {
        char _file[128];
        snprintf(_file, 128, "image%d.jpg", i+1);
        std::string file = FileUtils::getInstance()->fullPathForFilename(_file);
        data->images[i] = nvgCreateImage(vg, file.c_str(), 0);
        if (data->images[i] == 0) {
            printf("Could not load %s.\n", file.c_str());
            return -1;
        }
    }
    
    data->fontIcons = nvgCreateFont(vg, "icons", FileUtils::getInstance()->fullPathForFilename("entypo.ttf").c_str());
    if (data->fontIcons == -1) {
        printf("Could not add font icons.\n");
        return -1;
    }
    data->fontNormal = nvgCreateFont(vg, "sans", FileUtils::getInstance()->fullPathForFilename("Roboto-Regular.ttf").c_str());
    if (data->fontNormal == -1) {
        printf("Could not add font italic.\n");
        return -1;
    }
    data->fontBold = nvgCreateFont(vg, "sans-bold", FileUtils::getInstance()->fullPathForFilename("Roboto-Bold.ttf").c_str());
    if (data->fontBold == -1) {
        printf("Could not add font bold.\n");
        return -1;
    }
    
    return 0;
}

bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
//    Size visibleSize = Director::getInstance()->getVisibleSize();
//    Vec2 origin = Director::getInstance()->getVisibleOrigin();
//
//    /////////////////////////////
//    // 2. add a menu item with "X" image, which is clicked to quit the program
//    //    you may modify it.
//
//    // add a "close" icon to exit the progress. it's an autorelease object
//    auto closeItem = MenuItemImage::create(
//                                           "CloseNormal.png",
//                                           "CloseSelected.png",
//                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
//    
//	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
//                                origin.y + closeItem->getContentSize().height/2));
//
//    // create menu, it's an autorelease object
//    auto menu = Menu::create(closeItem, NULL);
//    menu->setPosition(Vec2::ZERO);
//    this->addChild(menu, 1);
//
//    /////////////////////////////
//    // 3. add your codes below...
//
//    // add a label shows "Hello World"
//    // create and initialize a label
//    
//    auto label = LabelTTF::create("Hello World", "Arial", 24);
//    
//    // position the label on the center of the screen
//    label->setPosition(Vec2(origin.x + visibleSize.width/2,
//                            origin.y + visibleSize.height - label->getContentSize().height));
//
//    // add the label as a child to this layer
//    this->addChild(label, 1);
//
//    // add "HelloWorld" splash screen"
//    auto sprite = Sprite::create("HelloWorld.png");
//
//    // position the sprite on the center of the screen
//    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
//
//    // add the sprite as a child to this layer
//    this->addChild(sprite, 0);
    
    cursorPoint=Vec2(0,0);
    premuto=false;
    
    auto listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(HelloWorld::onTouchCancelled, this);
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    struct timeval tv;
    gettimeofday(&tv,NULL);
    prevt = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    
    initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
    vg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    
    if (vg == NULL) {
        printf("Could not init nanovg.\n");
        return false;
    }
    
    if (loadDemoData(vg, &data) == -1)
        return false;
    
    return true;
}

bool HelloWorld::onTouchBegan(cocos2d::Touch *t, cocos2d::Event *e)
{
    premuto=true;
    cursorPoint=t->getLocationInView();
    return true;
}

void HelloWorld::onTouchEnded(cocos2d::Touch *t, cocos2d::Event *e)
{
    premuto=false;
    cursorPoint=t->getLocationInView();
}
void HelloWorld::onTouchMoved(cocos2d::Touch *t, cocos2d::Event *e)
{
    cursorPoint=t->getLocationInView();
}
void HelloWorld::onTouchCancelled(cocos2d::Touch *t, cocos2d::Event *e)
{
    premuto=false;
    cursorPoint=t->getLocationInView();
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
