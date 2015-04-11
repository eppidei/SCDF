//
//  LoadSavePanel.h
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#ifndef __ScdfController__LoadSavePanel__
#define __ScdfController__LoadSavePanel__

#include <stdio.h>
#include "Label.h"
#include "SCDFCWorkingPanel.h"


namespace ScdfCtrl
{
    class LoadSavePanelBase : public ModalPanel
    {
        void CreatePanel() override;
        virtual void CreateMain() = 0;
        virtual void CreateControlButton() = 0;
        virtual void OnTouchBegan(int nodeTag) = 0;
        virtual void OnTouchEnded(int nodeTag) = 0;
    protected:
        WorkingPanel *workingPanel;
        cocos2d::ui::Layout *mainPanel;
        cocos2d::ui::Button *control, *close;
        
    public:
        void OnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void SetWorkingPanel(WorkingPanel *wp) { workingPanel=wp;}
    };
    class SavePanel : public LoadSavePanelBase
    {
        cocos2d::ui::TextField* saveFile;
        
        void CreateMain() override;
        void CreateControlButton() override;
        void OnTouchBegan(int nodeTag) override;
        void OnTouchEnded(int nodeTag) override;
    public:
        CREATE_FUNC(SavePanel);
    };
    
    class LoadPanel : public LoadSavePanelBase
    {
        cocos2d::ui::ListView *loadFiles;

        void CreateMain() override;
        void CreateControlButton() override;
        void OnTouchBegan(int nodeTag) override;
        void OnTouchEnded(int nodeTag) override;
        void InitFilesListView();
        void HighLightCurrentItem();
    public:
        CREATE_FUNC(LoadPanel);
    };
}

#endif /* defined(__ScdfController__LoadSavePanel__) */
