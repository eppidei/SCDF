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
    
    class DropDownMenuCallbackPanel : public DropDownMenuCallback
    {
        PropertiesPanel *parent;
    public:
        DropDownMenuCallbackPanel(PropertiesPanel *_parent) : parent(_parent) {}
        void OnSizeChanged(float oldSize, float newSize);
    };
    
    class SubpanelBase : public cocos2d::ui::Layout
    {
        virtual void CreateControls() = 0;
    protected:
        PropertiesPanel *parent;
    public:
        ~SubpanelBase();
       // void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);
        virtual void PositionElements() {}
        virtual void UpdateValues() = 0;
        void InitWithContent(PropertiesPanel *parent, cocos2d::Size s);
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
            
            void TextFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
            void SelectedEvent(Ref* pSender,cocos2d::ui::CheckBox::EventType type);
        public:
            void UpdateValues();
            CREATE_FUNC(OSCInfo);
        } *sectionOSCInfo;
        
        class MIDIDevices : public SubpanelBase
        {
            DropDownMenu *devices;
            cocos2d::ui::Text *devicesLabel;
            
            void CreateControls();
        public:
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(MIDIDevices);
        } *sectionMIDIDevices;
        
        class MIDIInfo : public SubpanelBase
        {
            DropDownMenu *midiMessage, *controlChange, *channel, *velocity;
            cocos2d::ui::Text *midiMessageLabel, *controlChangeLabel, *channelLabel, *velocityLabel;
            
            void CreateControls();
        public:
            void PositionElements();
            void UpdateValues();
            CREATE_FUNC(MIDIInfo);
        } *sectionMIDIInfo;
        
        std::unique_ptr<DropDownMenuCallbackPanel> dropDownCallback;
        ScdfCtrl::ControlUnit *currentControlUnit;
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void PositionElements();
        void CalculateInnerHeight();
    public:
        DropDownMenuCallback *GetDropDownCallback();
        void InitLayout();
        void Update(SubjectSimple* theChangedSubject);
        static PropertiesPanel *CreatePropertiesPanel(MainScene *main, cocos2d::Rect r);
        CREATE_FUNC(PropertiesPanel);
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
