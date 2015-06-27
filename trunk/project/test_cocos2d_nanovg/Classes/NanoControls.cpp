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

void AudioPlotter::OnInit()
{
    isOn=false;
    c=cocos2d::Color4B(0,160,192,255);
    scdf::theSensorAPI()->InitFramework();
}
void AudioPlotter::OnTouchDown()
{
    isOn=!isOn;
    
    if (isOn)
    {
        std::vector<scdf::SensorType> _typeList;
        _typeList.push_back(scdf::AudioInput);
        scdf::theSensorAPI()->AttachHarvesterListener((scdf::HarvesterListener*)this, _typeList);
    }
    else
    {
        scdf::theSensorAPI()->DetachHarvesterListener((scdf::HarvesterListener*)this);
    }
}

void AudioPlotter::OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer)
{
    for (int i=0;i<buffer->size();++i)
    {
        if ((*buffer)[i]->type==scdf::AudioInput)
        {
            float *b=(float*)((*buffer)[i]->data);
            for(int j=0;j<(*buffer)[i]->num_frames;++j)
                data[j]=b[j];
            return;
        }
    }
}

void AudioPlotter::OnDraw()
{
    cocos2d::Rect viewRect=getBoundingBox();
    viewRect.origin=cocos2d::Vec2(0,0);
    
    std::vector<float> sx, sy;
    float dx = 0;
    
    int i;
    
    {
        scdf::ThreadUtils::AutoLock kk(&dataLock);
        if (0==data.size()) return;
        dx=viewRect.size.width/(data.size()-1);
        sx.resize(data.size());
        sy.resize(data.size());
        for (i = 0; i < data.size(); i++)
        {
            sx[i] = viewRect.origin.x+i*dx;
            sy[i] = viewRect.size.height*(1.0-(((data[i]+1.0)/2.0)));
        }
    }
    
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]);
    for (i = 1; i < sx.size(); i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
    nvgLineTo(GetNVGContext(), viewRect.origin.x+viewRect.size.width, viewRect.origin.y+viewRect.size.height);
    nvgLineTo(GetNVGContext(), viewRect.origin.x, viewRect.origin.y+viewRect.size.height);
    NVGpaint bg;
//    if(fillGraph)
//    {
//        bg = nvgLinearGradient(GetNVGContext(), viewRect.origin.x,viewRect.origin.y,viewRect.origin.x,viewRect.origin.y+viewRect.size.height, nvgRGBA(0,160,192,0), nvgRGBA(0,160,192,64));
//        
//    }
    nvgFillPaint(GetNVGContext(), bg);
    nvgFill(GetNVGContext());
    
    // Graph line
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]+2);
    for (i = 1; i < sx.size(); i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1]+2, sx[i]-dx*0.5f,sy[i]+2, sx[i],sy[i]+2);
    nvgStrokeColor(GetNVGContext(), nvgRGBA(0,0,0,32));
    nvgStrokeWidth(GetNVGContext(), 3.0f);
    nvgStroke(GetNVGContext());
    
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]);
    for (i = 1; i < sx.size(); i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
    nvgStrokeColor(GetNVGContext(), nvgRGBA(c.r,c.g,c.b,c.a));
    nvgStrokeWidth(GetNVGContext(), 1.0f);
    nvgStroke(GetNVGContext());
}

void Graph::OnInit()
{
    fillGraph=false;
}
void Graph::OnDraw()
{
    cocos2d::Rect viewRect=getBoundingBox();
    viewRect.origin=cocos2d::Vec2(0,0);
    
    float samples[6];
    float sx[6], sy[6];
    float dx = viewRect.size.width/((sizeof(samples)/sizeof(decltype(samples[0])))-1);
    int i;
    
    static int p=0;
    p=++p%(int)viewRect.size.height;
    samples[0] = 0.1;
    samples[1] = 0.3;
    samples[2] = 0.9;
    samples[3] = 0.5;
    samples[4] = 0.23;
    samples[5] = 0.44;
    
    for (i = 0; i < 6; i++) {
        sx[i] = viewRect.origin.x+i*dx;
        if (i==2)
            sy[i]=p;
        else
            sy[i] = viewRect.size.height*(1-samples[i]);
    }
    
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]);
    for (i = 1; i < 6; i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
    nvgLineTo(GetNVGContext(), viewRect.origin.x+viewRect.size.width, viewRect.origin.y+viewRect.size.height);
    nvgLineTo(GetNVGContext(), viewRect.origin.x, viewRect.origin.y+viewRect.size.height);
    NVGpaint bg;
    if(fillGraph)
    {
        bg = nvgLinearGradient(GetNVGContext(), viewRect.origin.x,viewRect.origin.y,viewRect.origin.x,viewRect.origin.y+viewRect.size.height, nvgRGBA(0,160,192,0), nvgRGBA(0,160,192,64));
        
    }
    nvgFillPaint(GetNVGContext(), bg);
    nvgFill(GetNVGContext());
    
    // Graph line
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]+2);
    for (i = 1; i < 6; i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1]+2, sx[i]-dx*0.5f,sy[i]+2, sx[i],sy[i]+2);
    nvgStrokeColor(GetNVGContext(), nvgRGBA(0,0,0,32));
    nvgStrokeWidth(GetNVGContext(), 3.0f);
    nvgStroke(GetNVGContext());
    
    nvgBeginPath(GetNVGContext());
    nvgMoveTo(GetNVGContext(), sx[0], sy[0]);
    for (i = 1; i < 6; i++)
        nvgBezierTo(GetNVGContext(), sx[i-1]+dx*0.5f,sy[i-1], sx[i]-dx*0.5f,sy[i], sx[i],sy[i]);
    nvgStrokeColor(GetNVGContext(), nvgRGBA(0,160,192,255));
    nvgStrokeWidth(GetNVGContext(), 1.0f);
    nvgStroke(GetNVGContext());
}
    