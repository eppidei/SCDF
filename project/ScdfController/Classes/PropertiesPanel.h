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
#include "Label.h"

namespace ScdfCtrl
{
    class ControlUnit;
    class MainScene;
    class PropertiesPanel;
    class ItemSlider;
    class ItemBase;
    class ItemBaseCallback;
    class PropertiesPanel;
    
    class OSCInfo : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        TextWithBackground *portLabel, *ipLabel, *oscTagLabel, *oscTag, *toggleLabel;
        cocos2d::ui::Button *oscToggle;
        TextInputWithBackground *oscPort, *oscIP;
        
        void UpdateOSCToggle();
        void CreateControls() override;
        void Update() override;
        void PositionElements() override;
        void CheckShowElements() override;
        void OnTouchEventBegan(cocos2d::Node *widget);
        void OnTextInput(cocos2d::ui::TextField *widget) override;
        int GetYPadding() override { return 20;}
        OSCInfo();
        CREATE_FUNC(OSCInfo);
    };
    
//    class MIDIDevices : public SubpanelBase
//    {
//        friend class PropertiesPanel;
//        
//        DropDownMenu *devices;
//        TextWithBackground *devicesLabel;
//        
//        void CreateControls() override;
//        void OnDropDownSelectionChange(DropDownMenu *menu) override;
//        void PositionElements() override;
//        void Update() override;
//        MIDIDevices();
//        int GetYPadding() override { return 0;}
//        CREATE_FUNC(MIDIDevices);
//    };
    
    class MIDIInfo : public SubpanelBase
    {
        friend class PropertiesPanel;
    
        DropDownMenu *midiMessage, *controlChange, *octaveMenu, *pitchValue, *programValue, *channel, *velocity, *midiOutDevices;
        TextWithBackground *outputDevicesLabel, *midiMessageLabel, *controlChangeLabel, *channelLabel, *velocityLabel, *midiLabel,*inputDevicesLabel;
        cocos2d::ui::Button *midiLearnToggle;
        
        void UpdateMidiLearnToggle();
        void CreateControls() override;
        void InitControlMenuData();
        void UpdateControlMenuData(MidiMessageType messageType);
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void CheckShowElements() override;
        void UpdateVelocity();
        int GetYPadding() override { return 20;}
        void OnTouchEventBegan(cocos2d::Node *widget) override;
        void UpdateElementsVisibilityOnMessageTypeChanged();
        void CheckMessageTypeDependentElementsVisibility(MidiMessageType messageType);
        void UpdateDevicesMenu();
        MIDIInfo();
        CREATE_FUNC(MIDIInfo);
    };
    
    class ItemSettings : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        float itemMultiply;
        cocos2d::ui::Button *h_plus, *h_minus, *w_plus, *w_minus;
        DropDownMenu *color, *orientation, *group, *modes;
        cocos2d::ui::Button *masterButton;
        TextInputWithBackground *name;
        TextWithBackground *sizeLabel, *colorLabel, *nameLabel, *settingsLabel, *orientLabel, *sizeText, *controlLabel, *groupLabel;

        void CheckAlgoTypePerItem(ItemBase *item);
        void CreateControls() override;
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void OnTextInput(cocos2d::ui::TextField *widget) override;
        void OnTouchEventBegan(cocos2d::Node *widget) override;
        void CheckShowElements() override;
        int GetYPadding() override { return 20;}
        ItemSettings();
        CREATE_FUNC(ItemSettings);
    };
    
    class PropertiesPanel : public PanelBase, public ObserverSimple
    {

        OSCInfo *sectionOSCInfo;
     //   MIDIDevices *sectionMIDIDevices;
        MIDIInfo *sectionMIDIInfo;
        ItemSettings *sectionItemSettings;
        ItemBase *selectedItem;
        
        void InitPanel() override;
        void UpdateSubpanels() override;
    public:
        void UpdateOSCInfo();
        void UpdateMidiInfo();
        void Update(SubjectSimple* theChangedSubject, SCDFC_EVENTS event) override;
        ScdfCtrl::MultiSender *GetCurrentSender();
        ItemBase *GetSelectedItem();
        void UpdateEditButton(bool editMode);
        void UpdateDevicesMenu();
        CREATE_FUNC(PropertiesPanel);
    protected:
        void OnHideShow();
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
