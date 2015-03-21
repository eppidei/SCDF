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
    class PropertiesPanel;
    
    class OSCInfo : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        cocos2d::ui::Text *toggleLabel, *portLabel, *ipLabel, *oscTagLabel, *oscTag;
        cocos2d::ui::CheckBox *oscToggle;
        cocos2d::ui::TextField* oscPort;
        cocos2d::ui::TextField* oscIP;
        
        void CreateControls() override;
        void Update() override;
        void PositionElements() override;
        void InitChildrensVisibilityAndPos() override;
        void OnCheckEvent(cocos2d::ui::CheckBox *widget, bool selected) override;
        void OnTextInput(cocos2d::ui::TextField *widget) override;
        OSCInfo();
        CREATE_FUNC(OSCInfo);
    };
    
    class MIDIDevices : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        DropDownMenu *devices;
        cocos2d::ui::Text *devicesLabel;
        
        void CreateControls() override;
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        MIDIDevices();
        CREATE_FUNC(MIDIDevices);
    };
    
    class MIDIInfo : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        DropDownMenu *midiMessage, *controlChange, *channel, *velocity;
        cocos2d::ui::Text *midiMessageLabel, *controlChangeLabel, *channelLabel, *velocityLabel;
        
        void CreateControls() override;
        void InitControlMenuValue();
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void UpdateVelocity();
        MIDIInfo();
        CREATE_FUNC(MIDIInfo);
    };
    
    class ItemSettings : public SubpanelBase
    {
        friend class PropertiesPanel;
        
        float itemMultiply;
        cocos2d::ui::Button *h_plus, *h_minus, *w_plus, *w_minus;
        cocos2d::ui::Text *sizeText;
        DropDownMenu *color;
        cocos2d::ui::TextField* name;
        cocos2d::ui::Text *sizeLabel, *colorLabel, *nameLabel;
        
        void CreateControls() override;
        void OnDropDownSelectionChange(DropDownMenu *menu) override;
        void PositionElements() override;
        void Update() override;
        void OnTextInput(cocos2d::ui::TextField *widget) override;
        void OnTouchEventBegan(cocos2d::Node *widget) override;
        ItemSettings();
        CREATE_FUNC(ItemSettings);
    };
    
    class PropertiesPanel : public PanelBase, public ObserverSimple
    {

        OSCInfo *sectionOSCInfo;
        MIDIDevices *sectionMIDIDevices;
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
