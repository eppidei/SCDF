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
#include "Panel.h"


namespace ScdfCtrl
{
    class LoadSavePanel;
    
    class SavePatch : public SubpanelBase
    {
        friend class LoadSavePanel;
        cocos2d::ui::Text *saveButton;
        cocos2d::ui::TextField* saveFile;
        
        void CreateControls() override;
        SavePatch();
        void PositionElements() override;
        int GetYPadding() override { return 0;}
        CREATE_FUNC(SavePatch);
    protected:
        void OnTouchEventBegan(Node *widget) override;
    };
    
    class LoadPatch : public SubpanelBase
    {
        friend class LoadSavePanel;
        cocos2d::ui::ListView *loadFiles;
        cocos2d::ui::Text *loadButton;
        
        void CreateControls() override;
        LoadPatch();
        void PositionElements() override;
        CREATE_FUNC(LoadPatch);
        void CheckShowElements() override {}
        void InitFilesListView();
        void HighLightCurrentItem();
        void Update() override;
        int GetYPadding() override { return 0;}
    protected:
        void OnTouchEventBegan(Node *widget) override;
        void OnTouchEventEnded(Node *widget) override;
    };
    
    class LoadSavePanel : public PanelBase
    {
        SavePatch *sectionSave;
        LoadPatch *sectionLoad;
        
        void UpdateSubpanels() override;
        void InitPanel() override;
    public:
        CREATE_FUNC(LoadSavePanel);
    };
}

#endif /* defined(__ScdfController__LoadSavePanel__) */
