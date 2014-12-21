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
#include "Observer.h"

namespace cocos2d{
    namespace ui{
        class CheckBox;
        class Layout;
        class TextField;
        class Text;
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

    class PropertiesPanel : public cocos2d::ui::ScrollView, public ObserverSimple
    {
        class OSCInfo : public cocos2d::ui::Layout
        {
            friend class PropertiesPanel;
            PropertiesPanel *parent;
            cocos2d::ui::Text *toggleLabel, *portLabel, *ipLabel, *oscTagLabel, *oscTag;
            cocos2d::ui::CheckBox *oscToggle;
            cocos2d::ui::TextField* oscPort;
            cocos2d::ui::TextField* oscIP;
            static int GetID() { return PROP_OSC_INFO_ID;}
            void CreateControls(PropertiesPanel *parent);
            void TextFieldEvent(Ref *pSender, cocos2d::ui::TextField::EventType type);
            void SelectedEvent(Ref* pSender,cocos2d::ui::CheckBox::EventType type);
            void UpdateValues();
        public:
            bool init() override;
            CREATE_FUNC(OSCInfo);
        } *sectionOSCInfo;
        
        friend class OSCInfo;
        ScdfCtrl::ControlUnit *currentControlUnit;
        void InitWithContent(MainScene *main, cocos2d::Rect r);
    public:
        void Update(SubjectSimple* theChangedSubject);
        static PropertiesPanel *CreatePropertiesPanel(MainScene *main, cocos2d::Rect r);
        CREATE_FUNC(PropertiesPanel);
    };
}
#endif /* defined(__ScdfController__PropertiesPanel__) */
