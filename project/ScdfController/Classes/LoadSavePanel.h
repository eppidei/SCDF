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


namespace ScdfCtrl
{
    class LoadSavePanelBaseCallback
    {
    public:
        virtual void OnLoadPatch(std::string patch)=0;
        virtual void OnSavePatch(std::string patch, bool newProject)=0;
        virtual void OnDiscardPatch()=0;
    };
    class LoadSavePanelBase : public ModalPanel
    {
        Node *CreatePanel() override;
        virtual void CreateMain() = 0;
        virtual void CreateControlButton() = 0;
        virtual void SetAdditionalButtonsPos(){}
        virtual void OnTouchBegan(int nodeTag) = 0;
        virtual void OnTouchEnded(int nodeTag) = 0;
    protected:
        LoadSavePanelBaseCallback *callback;
        cocos2d::ui::Layout *mainPanel;
        cocos2d::ui::Button *control, *discard;
        
    public:
        void OnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void SetCallback(LoadSavePanelBaseCallback *_callback) { callback=_callback;}
    };
    class SavePanel : public LoadSavePanelBase
    {
        void CreateMain() override;
        void OnTouchBegan(int nodeTag) override;
        void OnTouchEnded(int nodeTag) override;
    protected:
        cocos2d::ui::TextField* saveFile;
        void CreateControlButton() override;
    public:
        CREATE_FUNC(SavePanel);
    };
    class SaveAfterNewPanel : public SavePanel
    {
        void CreateControlButton() override;
        void SetAdditionalButtonsPos() override;
        void OnTouchBegan(int nodeTag) override;
    public:
        CREATE_FUNC(SaveAfterNewPanel);
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
