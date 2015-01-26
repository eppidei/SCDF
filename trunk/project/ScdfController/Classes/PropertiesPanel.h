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

namespace cocos2d{
    namespace ui{
        class CheckBox;
        class Layout;
        class TextField;
        class Text;
        class ListView;
    }
}

namespace ScdfCtrl{
class ControlUnit;
}

namespace SCDFC
{
#define PROP_OSC_INFO_ID 1
#define PROP_MIDI_OUT_ID 2
#define PROP_MIDI_INFO_ID 3
    
    class MainScene;
    class PropertiesPanel;
    class ItemSlider;
    class ItemBase;
    class ItemBaseCallback;
    
    class SubpanelBase : public cocos2d::ui::Layout, public cocos2d::ActionTweenDelegate
    {
        friend class PropertiesPanel;
        class DropDownMenuCallbackSubPanel : public DropDownMenuCallback
        {
            SubpanelBase *parent;
        public:
            DropDownMenuCallbackSubPanel(SubpanelBase *_parent) : parent(_parent) {}
            void OnSizeChanged(float oldSize, float newSize);
            void OnSelectItem(DropDownMenu *menu);
        };
        class SubPanelItemCallback : public ItemBaseCallback
        {
            SubpanelBase *parent;
        public:
            SubPanelItemCallback(SubpanelBase *_parent) : parent(_parent) {}
            void OnItemTouchBegan();
            void OnItemTouchMoved(int value);
            void OnItemTouchEnded();
        };
        
        void Resize(float newHeight);
        void CalculateHeight();
        void HideElement(Node *n, bool hide);
        virtual void CreateControls() = 0;
    protected:
        std::unique_ptr<PropertiesPanel> parent;
        std::unique_ptr<DropDownMenuCallbackSubPanel> dropDownCallback;
        std::unique_ptr<SubPanelItemCallback> itemCallback;
        
        void TextFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
        void TouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void CheckBoxEvent(Ref* pSender,cocos2d::ui::CheckBox::EventType type);
        virtual void InitChildrensVisibilityAndPos();
    public:
        ~SubpanelBase();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        virtual void PositionElements() = 0;
        virtual void OnDropDownSelectionChange(DropDownMenu *menu) {}
        virtual void UpdateValues() = 0;
        PropertiesPanel *GetParent();
        //DropDownMenuCallback *GetDropDownCallback();
        void updateTweenAction(float value, const std::string& key) override;
        void InitWithContent(PropertiesPanel *parent, cocos2d::Size s);
        //virtual bool init() override;
    };
    
    class PropertiesPanel : public cocos2d::ui::ScrollView, public ObserverSimple
    {
        class OSCInfo : public SubpanelBase
        {
            cocos2d::ui::Text *toggleLabel, *portLabel, *ipLabel, *oscTagLabel, *oscTag;
            cocos2d::ui::CheckBox *oscToggle;
            cocos2d::ui::TextField* oscPort;
            cocos2d::ui::TextField* oscIP;
            
            void CreateControls();
           // void TextFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
        protected:
            void InitChildrensVisibilityAndPos();
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
            cocos2d::ui::Layout *color;
            cocos2d::ui::TextField* name;
            cocos2d::ui::Text *sizeLabel, *colorLabel, *nameLabel;
            
            void CreateControls();
        public:
            ItemSettings();
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(ItemSettings);
        } *sectionItemSettings;
        
//        ScdfCtrl::ControlUnit *currentControlUnit;
        ItemBase *selectedItem;
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void PositionElements();
        void CalculateInnerHeight();
        void UpdateAll();
    public:
        void InitLayout();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
//        void OnSelectedDropDownItem(DropDownMenu *menu);
        void Update(SubjectSimple* theChangedSubject, SCDFC_EVENTS event);
        ScdfCtrl::ControlUnit *GetCurrentControlUnit();
        ItemBase *GetSelectedItem();
        static PropertiesPanel *CreatePropertiesPanel(MainScene *main, cocos2d::Rect r);
        void EnableScrolling(bool enable);
        CREATE_FUNC(PropertiesPanel);
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
