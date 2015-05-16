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
    class LoadSavePanelBase : public ModalPanel
    {
        Node *CreatePanel() override;
        virtual void CreateMain() = 0;
        virtual void CreateControlButton() = 0;
        virtual void SetAdditionalButtonsPos(){}
        virtual void OnTouchBegan(int nodeTag) = 0;
        virtual void OnTouchEnded(int nodeTag) = 0;
        virtual void SetCurrentPatchName(std::string name) {}
    protected:
        LoadSavePanelBaseCallback *callback;
        cocos2d::ui::Layout *mainPanel;
        cocos2d::ui::Button *control, *discard, *deleteBtn;
        
        virtual Node *GetMainControl() =0;
    public:
        void OnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void SetCallback(LoadSavePanelBaseCallback *_callback) override;
    };
    class SavePanel : public LoadSavePanelBase
    {
        void CreateMain() override;
        void OnTouchBegan(int nodeTag) override;
        void OnTouchEnded(int nodeTag) override;
        void CheckFileExists(std::string _text);
    protected:
        cocos2d::ui::TextField* saveFile;
        void CreateControlButton() override;
        void SetCurrentPatchName(std::string name) override;
        Node *GetMainControl() override {return saveFile;}
        void TextFieldEventCallback(Ref *pSender, cocos2d::ui::TextField::EventType type);
    public:
        CREATE_FUNC(SavePanel);
    };
    class SaveAfterNewPanel : public SavePanel
    {
        void CreateControlButton() override;
        void SetAdditionalButtonsPos() override;
        void OnTouchEnded(int nodeTag) override;
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
        void SetAdditionalButtonsPos() override;
    protected:
        Node *GetMainControl() override {return loadFiles;}
    public:
        CREATE_FUNC(LoadPanel);
    };
}

#endif /* defined(__ScdfController__LoadSavePanel__) */
