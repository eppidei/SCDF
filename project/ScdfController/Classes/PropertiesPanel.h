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
        void InitChildrensVisibilityAndPos() override;
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
    
        DropDownMenu *midiMessage, *controlChange, *channel, *velocity, *devices;
        TextWithBackground *devicesLabel, *midiMessageLabel, *controlChangeLabel, *channelLabel, *velocityLabel, *midiLabel;
        
        void CreateControls() override;
        void InitControlMenuValue();
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void InitChildrensVisibilityAndPos() override;
        void UpdateVelocity();
        int GetYPadding() override { return 20;}
        MIDIInfo();
        CREATE_FUNC(MIDIInfo);
    };
    
    class ItemSettings : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        float itemMultiply;
        cocos2d::ui::Button *h_plus, *h_minus, *w_plus, *w_minus;
        DropDownMenu *color, *orientation;
        TextInputWithBackground *name;
        TextWithBackground *sizeLabel, *colorLabel, *nameLabel, *settingsLabel, *orientLabel, *sizeText;
        
        void CreateControls() override;
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void OnTextInput(cocos2d::ui::TextField *widget) override;
        void OnTouchEventBegan(cocos2d::Node *widget) override;
        void InitChildrensVisibilityAndPos() override;
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
        void Update(SubjectSimple* theChangedSubject, SCDFC_EVENTS event) override;
        ScdfCtrl::MultiSender *GetCurrentSender();
        ItemBase *GetSelectedItem();
        CREATE_FUNC(PropertiesPanel);
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
