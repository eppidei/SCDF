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
        void OnTouchDown() override;
        void OnTouchUp() override;
        void OnTouchMoving() override;
        void OnDraw() override;
    };
}
#endif
