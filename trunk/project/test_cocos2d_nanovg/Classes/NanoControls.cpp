//
//  NanoControls.cpp
//  test_cocos2d_nanovg
//
//  Created by andrea scuderi on 26/06/15.
//
//

#include "NanoControls.h"
#include "nanovg.h"
#include "nanovg_gl.h"
#include "demo.h"

NanoControlBaseGlHelper::NanoControlBaseGlHelper() : nvgContext(nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES), nvgDeleteGLES2)
{
}

void NanoControlBaseGlHelper::EndPaint()
{
    nvgEndFrame(nvgContext.get());
    glEnable(GL_DEPTH_TEST);
}

void NanoControlBaseGlHelper::BeginPaint(cocos2d::Rect r)
{
    float winWidth=r.size.width;
    float winHeight=r.size.height;
    int fbWidth, fbHeight;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &fbWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &fbHeight);
    
    // Calculate pixel ration for hi-dpi devices.
    float pxRatio = ((float)fbWidth) / cocos2d::Director::getInstance()->getVisibleSize().width;
    
    // Update and render
    glViewport(r.origin.x*pxRatio, r.origin.y*pxRatio, winWidth*pxRatio, winHeight*pxRatio);
    
    //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    
    nvgBeginFrame(nvgContext.get(), winWidth, winHeight, pxRatio);
}


using namespace NanoControls;

int Demo::LoadDemoData(NVGcontext* vg, DemoData* data)
{
    int i;
    
    if (vg == NULL)
        return -1;
    
    for (i = 0; i < 12; i++) {
        char _file[128];
        snprintf(_file, 128, "image%d.jpg", i+1);
        std::string file = cocos2d::FileUtils::getInstance()->fullPathForFilename(_file);
        data->images[i] = nvgCreateImage(vg, file.c_str(), 0);
        if (data->images[i] == 0) {
            printf("Could not load %s.\n", file.c_str());
            return -1;
        }
    }
    
    data->fontIcons = nvgCreateFont(vg, "icons", cocos2d::FileUtils::getInstance()->fullPathForFilename("entypo.ttf").c_str());
    if (data->fontIcons == -1) {
        printf("Could not add font icons.\n");
        return -1;
    }
    data->fontNormal = nvgCreateFont(vg, "sans", cocos2d::FileUtils::getInstance()->fullPathForFilename("Roboto-Regular.ttf").c_str());
    if (data->fontNormal == -1) {
        printf("Could not add font italic.\n");
        return -1;
    }
    data->fontBold = nvgCreateFont(vg, "sans-bold", cocos2d::FileUtils::getInstance()->fullPathForFilename("Roboto-Bold.ttf").c_str());
    if (data->fontBold == -1) {
        printf("Could not add font bold.\n");
        return -1;
    }
    
    return 0;
}

void Demo::OnDestroy()
{
    freeDemoData(GetNVGContext(), data.get());
}
void Demo::OnInit()
{
    data.reset(new DemoData());
    LoadDemoData(GetNVGContext(), data.get());
}
void Demo::OnDraw()
{
    cocos2d::Size s=getContentSize();
    renderDemo(GetNVGContext(), lastCursorPoint.x, lastCursorPoint.y, s.width,s.height, 0, 0, data.get());
}