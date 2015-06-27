//
//  NanoControls.h
//  test_cocos2d_nanovg
//
//  Created by andrea scuderi on 27/06/15.
//
//

#ifndef test_cocos2d_nanovg_NanoControls_h
#define test_cocos2d_nanovg_NanoControls_h

#include "NanoControlBase.h"
#include "ScdfSensorAPI.h"
#include "ThreadUtils.h"

struct DemoData;

namespace NanoControls
{
    class Demo : public NanoControlBase<Demo>
    {
        std::unique_ptr<DemoData> data;
        int LoadDemoData(NVGcontext* vg, DemoData* data);
        void FreeDemoData();

        void OnInit() override;
        void OnTouchDown() override{}
        void OnTouchUp() override{}
        void OnTouchMoving() override{}
        void OnDraw() override;
        void OnDestroy() override;
    };

    class Graph : public NanoControlBase<Graph>
    {
        void OnInit() override;
        void OnTouchDown() override {}
        void SetFillGraph(bool fill) { fillGraph=fill;}
        void OnTouchUp() override {}
        void OnTouchMoving() override {}
        void OnDraw() override;
        
        bool fillGraph;
    };
    
    class AudioPlotter : public NanoControlBase<AudioPlotter>, public scdf::HarvesterListener
    {
    public:
        void SetData(std::vector<float> d)
        {
            scdf::ThreadUtils::AutoLock kk(&dataLock);
            data = d;
        }
        void SetColor(cocos2d::Color4B _c) { c=_c;}
        void OnHarvesterBufferReady(std::vector<scdf::SensorData*> *buffer);
        void Init(s_int32 numFrames, s_int32 rate) {data.resize(numFrames);}
        void Release(){}
    private:
        void OnDraw() override;
        void OnInit() override;
        void OnTouchDown() override;
        void OnTouchUp() override {}
        void OnTouchMoving() override {}
        
        bool isOn;
        std::vector<float> data;
        scdf::ThreadUtils::CustomMutex dataLock;
        cocos2d::Color4B c;
    };
}
#endif
