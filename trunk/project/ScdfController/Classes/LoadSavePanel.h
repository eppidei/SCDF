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
    
    class LoadSavePanel : public PanelBase
    {
        class SavePatch : public SubpanelBase
        {
            cocos2d::ui::Button *save;
            cocos2d::ui::TextField* saveFile;
            
            void CreateControls();
        protected:
            void InitChildrensVisibilityAndPos();
            void OnTouchEventBegan(cocos2d::ui::Button *widget);
        public:
            SavePatch();
            void UpdateValues();
            void PositionElements();
            CREATE_FUNC(SavePatch);
        } *sectionSave;
        
        class LoadPatch : public SubpanelBase
        {
            cocos2d::ui::ListView *files;
            cocos2d::ui::Button *load;
            
            void CreateControls();
        public:
            LoadPatch();
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(LoadPatch);
        } *sectionLoad;
        
        void InitPanel();
    public:
        CREATE_FUNC(LoadSavePanel);
    };
}

#endif /* defined(__ScdfController__LoadSavePanel__) */
