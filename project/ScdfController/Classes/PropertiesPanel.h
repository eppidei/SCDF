//
//  PropertiesPanel.h
//  ScdfController
//
//  Created by Andrea Scuderi on 21/12/14.
//
//

#ifndef __ScdfController__PropertiesPanel__
#define __ScdfController__PropertiesPanel__

#include "SCDFCDefinitions.h"
#include "DropDownMenu.h"
#include "Observer.h"
#include "Panel.h"

namespace ScdfCtrl
{
    class ControlUnit;
    class MainScene;
    class PropertiesPanel;
    class ItemSlider;
    class ItemBase;
    class ItemBaseCallback;
    
    class PropertiesPanel : public PanelBase, public ObserverSimple
    {
        class OSCInfo : public SubpanelBase
        {
            cocos2d::ui::Text *toggleLabel, *portLabel, *ipLabel, *oscTagLabel, *oscTag;
            cocos2d::ui::CheckBox *oscToggle;
            cocos2d::ui::TextField* oscPort;
            cocos2d::ui::TextField* oscIP;
            
            void CreateControls();
        protected:
            void InitChildrensVisibilityAndPos();
            void OnCheckEvent(cocos2d::ui::CheckBox *widget, bool selected);
            void OnTextInput(cocos2d::ui::TextField *widget);
        public:
            OSCInfo();
            void UpdateValues();
            void PositionElements();
            CREATE_FUNC(OSCInfo);
        } *sectionOSCInfo;
        
        class MIDIDevices : public SubpanelBase
        {
            DropDownMenu *devices;
            cocos2d::ui::Text *devicesLabel;
            
            void CreateControls();
        public:
            MIDIDevices();
            void OnDropDownSelectionChange(DropDownMenu *menu);
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(MIDIDevices);
        } *sectionMIDIDevices;
        
        class MIDIInfo : public SubpanelBase
        {
            DropDownMenu *midiMessage, *controlChange, *channel, *velocity;
            cocos2d::ui::Text *midiMessageLabel, *controlChangeLabel, *channelLabel, *velocityLabel;
            
            void CreateControls();
            void InitControlMenuValue();
        public:
            MIDIInfo();
            void OnDropDownSelectionChange(DropDownMenu *menu);
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(MIDIInfo);
        } *sectionMIDIInfo;
        
        class ItemSettings : public SubpanelBase
        {
            ItemSlider *sizeControl;
            DropDownMenu *color;
            cocos2d::ui::TextField* name;
            cocos2d::ui::Text *sizeLabel, *colorLabel, *nameLabel;
            
            void CreateControls();
        protected:
            void OnTextInput(cocos2d::ui::TextField *widget);
        public:
            ItemSettings();
            void OnDropDownSelectionChange(DropDownMenu *menu);
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(ItemSettings);
        } *sectionItemSettings;
        
        ItemBase *selectedItem;
        void InitPanel();
        void UpdateAll();
    public:
        void UpdateOSCInfo();
        void Update(SubjectSimple* theChangedSubject, SCDFC_EVENTS event);
        ScdfCtrl::ControlUnit *GetCurrentControlUnit();
        ItemBase *GetSelectedItem();
        CREATE_FUNC(PropertiesPanel);
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
