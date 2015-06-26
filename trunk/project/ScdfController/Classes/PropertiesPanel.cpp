//
//  PropertiesPanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 21/12/14.
//
//

#include "SCDFCItems.h"
#include "PropertiesPanel.h"
#include "LoadSavePanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "MainScene.h"
#include "MultiSender.h"
#include "SCDFCItems.h"
#include "OsUtilities.h"
#include "InAppPurchase.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

void CheckControlUnitPurchased(ControlUnit::Type cType)
{
    if (cType==ControlUnit::Wire) return;
    
    if(!CheckIsInAppPurchasedNoPrompt((PurchaseProductIndex)cType))
    {
        if (!CheckIsInAppPurchased((PurchaseProductIndex)cType))
            ModalPanel::CreateModalPanel("You can try the control type,\nbut you cannot use it with the MIDI/OSC sender");
    }
}
#define VISIBILITY_CHECK \
        if (NULL==panel->GetSelectedItem()) \
        {   \
            if (isVisible()) \
                setVisible(false); \
            return; \
        } \
        if (!isVisible()) \
            setVisible(true); \


void CreateLabelWithBackground(SubpanelBase *subPanel, TextWithBackground **label, int labelID, cocos2d::Rect r, std::string labelText, std::string labelFont, int fontSize, bool header=false)
{
    (*label)=TextWithBackground::CreateText(labelID,r, labelText,labelFont,fontSize);
    if (header)
    {
        (*label)->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
        (*label)->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::LabelHeaderBackGround));
        (*label)->AddTouchCallback(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, subPanel));
    }
    else
    {
        (*label)->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::GRADIENT);
        (*label)->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientTop),Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientBottom));
        if (-1!=labelID)
            (*label)->AddTouchCallback(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, subPanel));
    }
}

void DoPosition(Node *n, float xPos, float &yPos)
{
    if (n && n->isVisible())
    {
        n->setPosition(Vec2(xPos,yPos));
        yPos-=n->getContentSize().height;
    }
}

void OSCInfo::CheckShowElements()
{
    bool hideItem=collapsed;
    HideElement(portLabel,hideItem);
    HideElement(oscPort,hideItem);
    HideElement(ipLabel,hideItem);
    HideElement(oscIP,hideItem);
    HideElement(oscTagLabel,hideItem);
    HideElement(oscTag,hideItem);
    UpdateLayout();
}

OSCInfo::OSCInfo()
{
    toggleLabel=NULL;
    portLabel=ipLabel=oscTagLabel=oscTag=NULL;
    oscToggle=NULL;
    oscPort=oscIP=NULL;
}

void OSCInfo::CreateControls()
{
    cocos2d::Rect r(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT);
    
    //Create header
    CreateLabelWithBackground(this, &toggleLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "OSC", Colors::Instance()->GetFontPath(Colors::FontsId::PropHeader),Colors::Instance()->GetFontSize(Colors::FontsId::PropHeader), true);
    addChild(toggleLabel,0);
    
    //Create toggle
    oscToggle=Button::create();
    oscToggle->loadTextures("btnOFFSimple.png", "btnONSimple.png", "");
    oscToggle->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(cocos2d::Size(r.size.height,r.size.height));
    oscToggle->setAnchorPoint(Vec2(0,1));
    addChild(oscToggle,1,PROPERTIES_OSC_TOGGLE);
    oscToggle->ignoreContentAdaptWithSize(false);
    
    r.size.width-=GetYPadding();
    
    //Create oscPort control
    oscPort = TextInputWithBackground::CreateText(PROPERTIES_OSC_PORT, r, "No Port",Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    addChild(oscPort,2);
    oscPort->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscPort->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    oscPort->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    
    //Create oscPort label
    CreateLabelWithBackground(this, &portLabel, -1, r, "UDP PORT", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(portLabel,3);
    
    //Create oscIP control
    oscIP = TextInputWithBackground::CreateText(PROPERTIES_OSC_IP, r, "No Ip",Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    addChild(oscIP,4);
    oscIP->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscIP->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    oscIP->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));

    //Create oscIP label
    CreateLabelWithBackground(this, &ipLabel, PROPERTIES_OSC_IP, r, "IP ADDRESS", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(ipLabel,5);

    //Create OSCtag label
    CreateLabelWithBackground(this, &oscTagLabel, -1, r, "OSC TAG", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(oscTagLabel,6);
    
    //Create OSCtag label
    oscTag=TextWithBackground::CreateText(-1,r, "",Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    oscTag->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscTag->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    addChild(oscTag,7);
    
    PositionElements();
    CheckShowElements();
}

void OSCInfo::PositionElements()
{
    float yPos=getContentSize().height;
    const int xOffset=GetYPadding()/2;
    
    DoPosition(toggleLabel, 0, yPos);
    
    if (oscToggle)
        oscToggle->setPosition(Vec2(getContentSize().width-oscToggle->getContentSize().width,getContentSize().height));
    
    yPos-=xOffset;
    
    DoPosition(portLabel, xOffset, yPos);
    DoPosition(oscPort, xOffset, yPos);
    DoPosition(ipLabel, xOffset, yPos);
    DoPosition(oscIP, xOffset, yPos);
    DoPosition(oscTagLabel, xOffset, yPos);
    DoPosition(oscTag, xOffset, yPos);
}

void OSCInfo::UpdateOSCToggle()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL!=panel->GetCurrentSender() && panel->GetCurrentSender()->IsOscEnabled())
        oscToggle->loadTextureNormal("btnONSimple.png");
    else
        oscToggle->loadTextureNormal("btnOFFSimple.png");
}

void OSCInfo::Update()
{
    UpdateOSCToggle();
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (panel->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
    std::ostringstream os;
    os<<(panel->GetCurrentSender()->GetOscPort());
    oscPort->SetText(os.str());
    oscIP->SetText(panel->GetCurrentSender()->GetOscIp());
    oscTag->SetText(panel->GetCurrentSender()->GetOscTag());
}

void OSCInfo::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    switch (widget->getTag())
    {
        case PROPERTIES_OSC_TOGGLE:
        {
            bool change=true;
            if (panel->GetSelectedItem()->GetControlUnit()->GetType()!=ControlUnit::Wire)
                change=CheckIsInAppPurchased((PurchaseProductIndex)panel->GetSelectedItem()->GetControlUnit()->GetType());
            
            if (!change) break;

            panel->GetCurrentSender()->SetOscEnabled(!panel->GetCurrentSender()->IsOscEnabled());
            UpdateOSCToggle();
            break;
        }
        case PROPERTIES_OSC_PORT:
            oscPort->OnTouchIndirect();
            break;
        case PROPERTIES_OSC_IP:
            oscIP->OnTouchIndirect();
            break;
            
        default:
            break;
    }
}

void OSCInfo::OnTextInput(cocos2d::ui::TextField *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
 
    switch (widget->getTag())
    {
        case PROPERTIES_OSC_PORT:
            panel->GetCurrentSender()->SetOscPort(std::stoi(widget->getStringValue()));
            break;
        case PROPERTIES_OSC_IP:
            panel->GetCurrentSender()->SetOscIp(widget->getStringValue());
            break;
        default:
            break;
    }
}

void MIDIInfo::UpdateVelocity()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel) return;
    if (dynamic_cast<ItemPad*>(panel->GetSelectedItem())!=NULL)
        velocity->SetSelectedIndex(panel->GetSelectedItem()->GetControlUnit()->GetMax());
    else
        velocity->SetSelectedIndex(panel->GetSelectedItem()->GetControlUnit()->GetValue());
    EnableElement(velocity, dynamic_cast<ItemSlider*>(panel->GetSelectedItem())==NULL && dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL);
    EnableElement(velocityLabel, dynamic_cast<ItemSlider*>(panel->GetSelectedItem())==NULL && dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL);
}

void MIDIInfo::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    VISIBILITY_CHECK
    
    if (NULL==panel->GetCurrentSender()) return;
    
    devices->SetSelectedIndex(panel->GetCurrentSender()->GetMidiOutIndex()+1);
    
    int selectedIndex=-1;
    switch(panel->GetCurrentSender()->GetMidiMessageType())
    {
        case NoteOn: selectedIndex=0; break;
        case NoteOff: selectedIndex=1; break;
        case Aftertouch: selectedIndex=2; break;
        case ControlChange: selectedIndex=3; break;
        case ProgramChange: selectedIndex=4; break;
        case PolyKeyPressure: selectedIndex=5; break;
        case PitchBend: selectedIndex=6; break;
        default: break;
    }
    if (-1!=selectedIndex)
        midiMessage->SetSelectedIndex(selectedIndex);
    UpdateElementsVisibilityOnMessageTypeChanged();

    channel->SetSelectedIndex(panel->GetCurrentSender()->GetMidiChannel()+1);
    UpdateVelocity();
}

void MIDIInfo::PositionElements()
{
    float yPos=getContentSize().height;
    const int xOffset=GetYPadding()/2;
    
    DoPosition(midiLabel, 0, yPos);
    
    yPos-=xOffset;
    
    DoPosition(devicesLabel, xOffset, yPos);
    DoPosition(devices, xOffset, yPos);
    DoPosition(midiMessageLabel, xOffset, yPos);
    DoPosition(midiMessage, xOffset, yPos);
    DoPosition(controlChangeLabel, xOffset, yPos);

    DoPosition(controlChange, xOffset, yPos);
    DoPosition(octaveMenu, xOffset, yPos);
    DoPosition(pitchValue, xOffset, yPos);
    DoPosition(programValue, xOffset, yPos);
    
    DoPosition(channelLabel, xOffset, yPos);
    DoPosition(channel, xOffset, yPos);
    DoPosition(velocityLabel, xOffset, yPos);
    DoPosition(velocity, xOffset, yPos);
}

void MIDIInfo::CheckShowElements()
{
    HideElement(devicesLabel,collapsed);
    HideElement(devices,collapsed);
    UpdateElementsVisibilityOnMessageTypeChanged();
    UpdateLayout();
}

std::string GetCCName(int ccNumber)
{
    switch (ccNumber)
    {
        case 0: return " - Bank";
        case 1: return " - Mod Wheel";
        case 2: return " - Breath";
        case 4: return " - Foot";
        case 5: return " - Portamento";
        case 7: return " - Volume";
        case 8: return " - Balance";
        case 10: return " - Pan";
        case 11: return " - Expression";
        case 64: return " - Damper on/off";
        case 65: return " - Portamento on/off";
        case 66: return " - Sostenuto on/off";
        case 67: return " - Soft Pedal on/off";
        case 68: return " - Legato on/off";
        case 69: return " - Hold on/off";
        default: return "";
    }
}
void MIDIInfo::InitControlMenuData()
{
    std::vector<DropDownMenuData> dropDownData;

    static std::string note[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    
    int octave=-1;
    for (int i=0;i<127;++i)
    {
        std::ostringstream os;
        os<<note[i%12]<<octave;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
        if (i>0&&0==i%12)
            octave++;
    }
    pitchValue->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    pitchValue->setVisible(false);
    
    dropDownData.clear();
    for (int i=0;i<120;++i)
    {
        std::ostringstream os;
        os<<"CC: "<<i<<GetCCName(i);
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    controlChange->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    controlChange->setVisible(true);
    
    dropDownData.clear();
    for (int i=0;i<11;++i)
    {
        std::ostringstream os;
        os<<i-1;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }

    octaveMenu->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    octaveMenu->setVisible(false);
    
//    dropDownData.clear();
//    for (int i=0;i<128;++i)
//    {
//        std::ostringstream os;
//        os<<i;
//        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
//    }
//    programValue->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
//    programValue->setVisible(false);
    
}
void MIDIInfo::CheckMessageTypeDependentElementsVisibility(MidiMessageType messageType)
{
    HideElement(channelLabel, false);
    HideElement(channel,false);
    HideElement(controlChangeLabel,false);
    
    UpdateControlMenuData(messageType);
    
    if (messageType!=PitchBend)
    {
        HideElement(velocityLabel,false);
        HideElement(velocity, false);
    }
}

void MIDIInfo::UpdateElementsVisibilityOnMessageTypeChanged()
{
    HideElement(midiMessage, true);
    HideElement(midiMessageLabel,true);
    HideElement(channelLabel, true);
    HideElement(channel,true);
    HideElement(controlChangeLabel,true);
    
    HideElement(velocityLabel,true);
    HideElement(velocity, true);
    HideElement(octaveMenu, true);
    HideElement(controlChange,true);
    HideElement(pitchValue,true);
    HideElement(programValue,true);
    
    if (collapsed) return;
    
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    
    CheckMessageTypeDependentElementsVisibility(panel->GetCurrentSender()->GetMidiMessageType());
    UpdateLayout();
}

void MIDIInfo::UpdateControlMenuData(MidiMessageType messageType)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    
    velocityLabel->SetText("VELOCITY");
    HideElement(controlChangeLabel, false);
    if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL)
    {
        HideElement(midiMessage, false);
        HideElement(midiMessageLabel, false);
        
        DropDownMenu *menu;
        switch(messageType)
        {
            case NoteOn:
            case NoteOff:
                controlChangeLabel->SetText("NOTE");
                menu=pitchValue;
                break;
            case PolyKeyPressure:
                controlChangeLabel->SetText("PRESSURE");
                velocityLabel->SetText("PRESSURE");
                menu=pitchValue;
                break;
            case ControlChange:
                controlChangeLabel->SetText("CONTROL CHANGE");
                velocityLabel->SetText("VALUE");
                menu=controlChange;
                break;
            case ProgramChange:
                velocityLabel->SetText("PROGRAM");
                HideElement(controlChangeLabel, true);
                menu=NULL;
                break;
            case Aftertouch:
                velocityLabel->SetText("PRESSURE");
                HideElement(controlChangeLabel, true);
                menu=NULL;
                break;
            case PitchBend:
                HideElement(controlChangeLabel, true);
                menu=NULL;
                break;
            default: break;
        }
        if (menu){
            HideElement(menu,false);
            menu->SetSelectedIndex(panel->GetCurrentSender()->GetMidiControl());
        }
    }
    else
    {
        HideElement(octaveMenu,false);
        controlChangeLabel->SetText("OCTAVE");
        octaveMenu->SetSelectedIndex(dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())->GetCurrentOctave());
    }
}

void MIDIInfo::OnDropDownSelectionChange(DropDownMenu *menu)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    int selectedIndex=menu->getCurSelectedIndex();
    if (menu==midiMessage)
    {
        MidiMessageType msg;
        switch(selectedIndex)
        {
            case 0: msg=NoteOn; break;
            case 1: msg=NoteOff; break;
            case 2: msg=Aftertouch; break;
            case 3: msg=ControlChange; break;
            case 4: msg=ProgramChange; break;
            case 5: msg=PolyKeyPressure; break;
            case 6: msg=PitchBend; break;
            default: return;
        }
        panel->GetSelectedItem()->GetControlUnit()->SetMidiMessageType(msg);
        UpdateElementsVisibilityOnMessageTypeChanged();
    }
    else if (menu==controlChange || menu==pitchValue || menu==programValue)
        panel->GetCurrentSender()->SetMidiControl(selectedIndex);
    else if (menu==octaveMenu)
        dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())->SetCurrentOctave(selectedIndex);
    else if (menu==channel)
        panel->GetCurrentSender()->SetMidiChannel(selectedIndex-1);
    else if (menu==velocity)
        panel->GetSelectedItem()->GetControlUnit()->SetMax(selectedIndex);
    	// this is used for the button, and the button
    	// sends the max value when pressed
    else if (menu==devices)
    {
        bool change=true;
        if (panel->GetSelectedItem()->GetControlUnit()->GetType()!=ControlUnit::Wire)
            change=CheckIsInAppPurchased((PurchaseProductIndex)panel->GetSelectedItem()->GetControlUnit()->GetType());
        if (change)
            panel->GetCurrentSender()->SetMidiOutIndex(selectedIndex-1);
        Update();
    }

    panel->UpdateOSCInfo();
}

MIDIInfo::MIDIInfo()
{
    devices=NULL;
    devicesLabel=NULL;
    midiMessage=controlChange=channel=octaveMenu=pitchValue=programValue=velocity=NULL;
    midiMessageLabel=controlChangeLabel=channelLabel=velocityLabel=midiLabel=NULL;
}

void MIDIInfo::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;

    switch (widget->getTag())
    {
        case PROPERTIES_MIDI_DEVICE: devices->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_MIDI_MESSAGE: midiMessage->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_MIDI_CONTROL:
        {
            DropDownMenu *menu;
            if (controlChange->isVisible())
                menu=controlChange;
            else if (octaveMenu->isVisible())
                menu=octaveMenu;
//            else if (programValue->isVisible())
//                menu=programValue;
            else
                menu=pitchValue;
            menu->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
        }
            break;
        case PROPERTIES_MIDI_CHANNEL: channel->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_MIDI_VELOCITY: velocity->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        default:
            break;
            
            
    }
}

void MIDIInfo::UpdateDevicesMenu()
{
    std::vector<DropDownMenuData> dropDownData;
    dropDownData.push_back(DropDownMenuData("No MIDI connection",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(DropDownMenuData(Scdf::MidiOutConnection::GetOutputName(i),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    devices->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
}

void MIDIInfo::CreateControls()
{
    cocos2d::Rect r(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT);
    
    //Create header
    CreateLabelWithBackground(this, &midiLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "MIDI", Colors::Instance()->GetFontPath(Colors::FontsId::PropHeader),Colors::Instance()->GetFontSize(Colors::FontsId::PropHeader), true);
    addChild(midiLabel);
    
    r.size.width-=GetYPadding();

    //Create device label
    CreateLabelWithBackground(this, &devicesLabel, PROPERTIES_MIDI_DEVICE, r, "DEVICES", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(devicesLabel);
    
    //Create device dropDown
    devices = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(devices);
    UpdateDevicesMenu();
    
    //Create midiMessage label
    CreateLabelWithBackground(this, &midiMessageLabel, PROPERTIES_MIDI_MESSAGE, r, "MESSAGE", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(midiMessageLabel);
    
    //Create midiMessage dropDown
    std::vector<std::string> MidiMessageString;
    MidiMessageString.push_back("Note On");
    MidiMessageString.push_back("Note Off");
    MidiMessageString.push_back("Aftertouch");
    MidiMessageString.push_back("Control Change");
    MidiMessageString.push_back("Program Change");
    MidiMessageString.push_back("PolyKeyPressure");
    MidiMessageString.push_back("Pitch Bend");
    midiMessage = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(midiMessage);
    
    std::vector<DropDownMenuData> dropDownData;
    for (int i=0;i<MidiMessageString.size();++i)
        dropDownData.push_back(DropDownMenuData(MidiMessageString[i],Colors::Instance()->GetUIColor(Colors::DropDownText)));
    midiMessage->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create ControlChange label
    CreateLabelWithBackground(this, &controlChangeLabel, PROPERTIES_MIDI_CONTROL, r, "CONTROL CHANGE", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(controlChangeLabel);

    //Create ControlChange dropDown
    controlChange = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(controlChange);
    octaveMenu = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(octaveMenu);
    pitchValue = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(pitchValue);
//    programValue = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
//    addChild(programValue);
    InitControlMenuData();
    
    //Create channel label
    CreateLabelWithBackground(this, &channelLabel, PROPERTIES_MIDI_CHANNEL, r, "CHANNEL", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(channelLabel);

    //Create channel dropDown
    channel = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(channel);
    dropDownData.clear();
    dropDownData.push_back(DropDownMenuData("Any",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    const int NUM_MIDI_CHANNELS = 16;
    for (int i=1;i<=NUM_MIDI_CHANNELS;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    channel->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);

    //Create velocity label
    CreateLabelWithBackground(this, &velocityLabel, PROPERTIES_MIDI_VELOCITY, r, "VELOCITY", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(velocityLabel);
    
    //Create velocity dropDown
    velocity = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(velocity);
    dropDownData.clear();
    const int NUM_MIDI_NOTES = 128;
    for (int i=0;i<NUM_MIDI_NOTES;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    velocity->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
}

ItemSettings::ItemSettings()
{
    h_minus=h_plus=w_minus=w_plus=NULL;
    sizeText=NULL;
    color=orientation=group=NULL;
    name=NULL;
    sizeLabel=colorLabel=nameLabel=groupLabel=NULL;
    orientLabel=NULL;
    masterButton=NULL;
    modes=NULL;
    
}

void ItemSettings::CheckShowElements()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    HideElement(nameLabel,collapsed);
    HideElement(name,collapsed);

    HideElement(group,collapsed);
    HideElement(groupLabel,collapsed);
    HideElement(masterButton,collapsed || group->GetSelectedIndex()==0);

    bool forceHide=false;
    if (panel->GetSelectedItem()&&panel->GetSelectedItem()->GetID()==ITEM_KEYBOARD_ID)
        forceHide=true;
    
    HideElement(colorLabel,collapsed||forceHide);
    HideElement(color,collapsed||forceHide);
    HideElement(controlLabel,collapsed||forceHide);
    HideElement(modes,collapsed||forceHide);
    HideElement(sizeLabel,collapsed||forceHide);
    HideElement(h_plus,collapsed||forceHide);
    HideElement(sizeText,collapsed||forceHide);
    HideElement(h_minus,collapsed||forceHide);
    
    forceHide=false;
    if ((panel->GetSelectedItem()&&panel->GetSelectedItem()->GetID()==ITEM_PAD_ID) ||
        (panel->GetSelectedItem()&&panel->GetSelectedItem()->GetID()==ITEM_KNOB_ID))
        forceHide=true;
    
    HideElement(orientation,collapsed||forceHide);
    HideElement(orientLabel,collapsed||forceHide);
    UpdateLayout();
}

void ItemSettings::PositionElements()
{
    float yPos=getContentSize().height;
    const int xOffset=GetYPadding()/2;
    
    DoPosition(settingsLabel, 0, yPos);
    
    yPos-=xOffset;
    
    DoPosition(nameLabel, xOffset, yPos);
    DoPosition(name, xOffset, yPos);
    DoPosition(colorLabel, xOffset, yPos);
    if (color && color->isVisible())
    {
        yPos-=SUBPANEL_ITEM_HEIGHT/3;
        DoPosition(color, xOffset, yPos);
        yPos-=SUBPANEL_ITEM_HEIGHT/3;
    }
    DoPosition(controlLabel, xOffset, yPos);
    DoPosition(modes, xOffset, yPos);
    DoPosition(groupLabel, xOffset, yPos);
    DoPosition(group, xOffset, yPos);
    DoPosition(masterButton, xOffset, yPos);
    DoPosition(orientLabel, xOffset, yPos);
    DoPosition(orientation, xOffset, yPos);
    DoPosition(sizeLabel, xOffset, yPos);
    if (h_plus)
    {
        h_plus->setPosition(Vec2(getContentSize().width-xOffset-h_plus->getContentSize().width,yPos));
        sizeText->setPosition(Vec2(xOffset,yPos));
        h_minus->setPosition(Vec2(xOffset,yPos));
    }
}

void ItemSettings::CreateControls()
{
    cocos2d::Rect r(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT);
    
    //Create header
    CreateLabelWithBackground(this, &settingsLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "ITEM SETTINGS", Colors::Instance()->GetFontPath(Colors::FontsId::PropHeader),Colors::Instance()->GetFontSize(Colors::FontsId::PropHeader), true);
    addChild(settingsLabel,1);
    
    r.size.width-=GetYPadding();

    //Create name label
    CreateLabelWithBackground(this, &nameLabel, PROPERTIES_ITEM_NAME, r, "NAME", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(nameLabel,2);
    
    //Create name control
    name = TextInputWithBackground::CreateText(PROPERTIES_ITEM_NAME, r, "No Name",Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    addChild(name,3);
    name->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    name->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    name->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    
    //Create color label
    CreateLabelWithBackground(this, &colorLabel, PROPERTIES_ITEMSETTINGS_COLOR, r, "COLOR", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(colorLabel,4);
    
    //Create color control
    color = DropDownMenu::CreateMenu<DropDownColorMenu>(r.size, dropDownCallback.get());
    addChild(color,5,PROPERTIES_ITEM_COLOR);
//    color->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::SubpanelGenericItem));
    color->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::NONE);
    
    std::vector<DropDownMenuData> dropDownData;
    for (int i=0;i<Colors::Instance()->CountItemsColor();++i)
        dropDownData.push_back(DropDownMenuData("",Colors::Instance()->GetItemsColor((Colors::ItemsColorsId)i)));
    
    color->InitData(dropDownData, 2.0*SUBPANEL_ITEM_HEIGHT/3.0);
    
    //create control modes label
    CreateLabelWithBackground(this, &controlLabel, PROPERTIES_ITEMSETTINGS_MODES, r, "CONTROL MODE", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(controlLabel,6);
    
    //create control modes control

    modes = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(modes,7);
    dropDownData.clear();
    dropDownData.push_back(DropDownMenuData("Touch",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    dropDownData.push_back(DropDownMenuData("Blow",Colors::Instance()->GetUIColor(Colors::DropDownText)));
#ifdef _DEBUG
    dropDownData.push_back(DropDownMenuData("Snap",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    dropDownData.push_back(DropDownMenuData("Proximity",Colors::Instance()->GetUIColor(Colors::DropDownText)));
#endif
    //dropDownData.push_back(DropDownMenuData("Proximity",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    modes->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create group label
    CreateLabelWithBackground(this, &groupLabel, PROPERTIES_ITEMSETTINGS_GROUP, r, "GROUP", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(groupLabel,8);
    
    //Create orient dropDown
    group = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(group,9);
    dropDownData.clear();
    dropDownData.push_back(DropDownMenuData("No Group",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    for(int i=0;i<11;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    group->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create master toggle
    masterButton=Button::create();
    masterButton->loadTextures("groupMasterBtnDefault.png", "groupMasterBtnDefault.png", "");
    masterButton->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    masterButton->setTouchEnabled(true);
    masterButton->setContentSize(cocos2d::Size(r.size.width,r.size.height));
    masterButton->setAnchorPoint(Vec2(0,1));
    addChild(masterButton,10,PROPERTIES_ITEMSETTINGS_MASTER);
    masterButton->ignoreContentAdaptWithSize(false);
    
    //Create orient label
    CreateLabelWithBackground(this, &orientLabel, PROPERTIES_ITEMSETTINGS_ORIENT, r, "ORIENTATION", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(orientLabel,11);
    
    //Create orient dropDown
    orientation = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(orientation,12);
    orientation->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    dropDownData.clear();
    dropDownData.push_back(DropDownMenuData("Vertical",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    dropDownData.push_back(DropDownMenuData("Horizontal",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    orientation->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create size label
    CreateLabelWithBackground(this, &sizeLabel, -1, r, "SIZE", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenuLabel),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenuLabel));
    addChild(sizeLabel,13);
    
    //Create size control
    h_minus = ui::Button::create();
    addChild(h_minus, 15, PROPERTIES_ITEM_HEIGHT_MINUS);
    h_minus->setTouchEnabled(true);
    h_minus->ignoreContentAdaptWithSize(false);
    h_minus->loadTextures("zoomOutDefault.png", "zoomOutActive.png", "");
    h_minus->setAnchorPoint(Vec2(0,1));
    h_minus->setContentSize(cocos2d::Size(r.size.height, r.size.height));
    h_minus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    
    CreateLabelWithBackground(this, &sizeText, -1, r, "", Colors::Instance()->GetFontPath(Colors::FontsId::DropDownMenu),Colors::Instance()->GetFontSize(Colors::FontsId::DropDownMenu));
    sizeText->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    sizeText->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    addChild(sizeText,14);
    sizeText->SetAlignement(TextHAlignment::CENTER, TextVAlignment::CENTER);
    
    h_plus = ui::Button::create();
    addChild(h_plus, 16, PROPERTIES_ITEM_HEIGHT_PLUS);
    h_plus->ignoreContentAdaptWithSize(false);
    h_plus->setTouchEnabled(true);
    h_plus->loadTextures("zoomInDefault.png", "zoomInActive.png", "");
    h_plus->setAnchorPoint(Vec2(0,1));
    h_plus->setContentSize(cocos2d::Size(r.size.height, r.size.height));
    h_plus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
}

void ItemSettings::CheckAlgoTypePerItem(ItemBase *item)
{
    bool isContinuousControl=dynamic_cast<ItemSlider*>(item)!=NULL;
    bool proximitySensorExists=scdf::theSensorAPI()->SensorExists(scdf::Proximity);
    modes->HideItem(ControlUnit::Snap, isContinuousControl);
    modes->HideItem(ControlUnit::Proximity, isContinuousControl || !proximitySensorExists);
}

void ItemSettings::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    VISIBILITY_CHECK
    
    ItemBase *item=panel->GetSelectedItem();
    if(NULL==item) return;
    std::ostringstream os;
    char str[256];
    int magValue=item->GetLayoutManager()->GetMagValue();
    if (magValue==0)
        sprintf(str,"Min");
    else if (magValue==8)
        sprintf(str,"Max");
    else
        sprintf(str,"+ %d",item->GetLayoutManager()->GetMagValue());
    sizeText->SetText(str);
    color->SetSelectedIndex(item->GetColor());
    name->SetText(item->GetName());
//    modes->CheckButton((int)(item->GetControlUnit()->GetType())+PROPERTIES_CONTROLMODE_BASE);
    modes->SetSelectedIndex(item->GetControlUnit()->GetType());
    int orientationIndex=1;
    if (item->GetLayoutManager()->IsVertical())
        orientationIndex=0;
    orientation->SetSelectedIndex(orientationIndex);
    group->SetSelectedIndex(item->GetGroupID()+1);
    if (item->IsMaster())
        masterButton->setColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    else
        masterButton->setColor(Color3B::WHITE);

    CheckAlgoTypePerItem(item);
    CheckShowElements();
    
}

void ItemSettings::OnDropDownSelectionChange(DropDownMenu *menu)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
    if (color==menu)
    {
        panel->GetSelectedItem()->SetColor((Colors::ItemsColorsId)menu->GetSelectedIndex());
        Update();
    }
    else if (orientation==menu)
    {
        panel->GetSelectedItem()->GetLayoutManager()->SetVertical(menu->GetSelectedIndex()==0);
        Update();
    }
    else if (group==menu)
    {
        panel->GetSelectedItem()->SetGroupID(menu->GetSelectedIndex()-1);
        Update();
    }
    else if (modes==menu)
    {
        ControlUnit::Type controlUnitType=(ControlUnit::Type)menu->GetSelectedIndex();
        CheckControlUnitPurchased(controlUnitType);
        panel->GetSelectedItem()->ChangeControlUnit(controlUnitType);
        Update();
    }
    
}

void ItemSettings::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
    bool update=true;
    switch (widget->getTag())
    {
        case PROPERTIES_ITEM_NAME:
            name->OnTouchIndirect();
            break;
        case PROPERTIES_ITEM_HEIGHT_MINUS:
            panel->GetSelectedItem()->GetLayoutManager()->ZoomMinus();
            break;
        case PROPERTIES_ITEM_HEIGHT_PLUS:
            panel->GetSelectedItem()->GetLayoutManager()->ZoomPlus();
            break;
        case PROPERTIES_ITEMSETTINGS_MASTER:
            panel->GetSelectedItem()->SetMaster(!panel->GetSelectedItem()->IsMaster());
            break;
        case PROPERTIES_ITEMSETTINGS_GROUP:
            group->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_ITEMSETTINGS_COLOR:
            color->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_ITEMSETTINGS_MODES:
            modes->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        case PROPERTIES_ITEMSETTINGS_ORIENT:
            orientation->OnControlTouch(NULL, ListView::EventType::ON_SELECTED_ITEM_END);
            break;
        default:
            update=false;
            break;
    }
    if (update)
    Update();
}

void ItemSettings::OnTextInput(cocos2d::ui::TextField *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
    
    switch (widget->getTag())
    {
        case PROPERTIES_ITEM_NAME:
            panel->GetSelectedItem()->SetName(widget->getStringValue());
            break;
        default:
            break;
    }
}

void PropertiesPanel::InitPanel()
{
    const float bitmapTopTransparencyPercentage=0.010;
    const float tongueTopTransparencyPercentage=0.087;
    const float bitmapBottomTransparencyPercentage=0.022;
    const float bitmapToolbarHeightPercentage=0.058;
    const float bitmapToolbarWidthPercentage=1.0-PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE;
    const float numButtons=4.0;
    
    float ypadding=(getContentSize().height*bitmapToolbarHeightPercentage)*0.1;
   
    float buttonPlaceholder=(bitmapToolbarWidthPercentage*getContentSize().width)/numButtons;//-(numButtons+1)*xpadding;
    float buttonDim=0.7*buttonPlaceholder;
    float xpadding=((bitmapToolbarWidthPercentage*getContentSize().width)-numButtons*buttonDim)/(numButtons+1.0);
    
    const float tongueBiggerFactor=1.3;
    cocos2d::Size arrowButtonSize=cocos2d::Size(1.22*buttonDim*tongueBiggerFactor, 1.26*buttonDim*tongueBiggerFactor);
    
    float buttonYPos=getContentSize().height*(1.0-bitmapTopTransparencyPercentage) - ypadding;
    float buttonXPos=xpadding;
    
    setBackGroundImage("panelLeft.png");
    cocos2d::Rect rr(0, 0, getBackGroundImageTextureSize().width, getBackGroundImageTextureSize().height);
    setBackGroundImageScale9Enabled(true);
    setBackGroundImageCapInsets(rr);

    float scrollbarHeight=getContentSize().height*(1.0-2.0*(bitmapTopTransparencyPercentage+bitmapBottomTransparencyPercentage+bitmapToolbarHeightPercentage));
    float scrollbarWidth=getContentSize().width*(1.0-(PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE))-2*xpadding;
    scrollView->setContentSize(cocos2d::Size(scrollbarWidth, scrollbarHeight));
    scrollView->setPosition(Vec2(xpadding,buttonYPos-1.5*buttonDim));//(1.0-1.5*(bitmapTopTransparencyPercentage+bitmapToolbarHeightPercentage))*getContentSize().height));
    
    selectedItem=NULL;
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionOSCInfo);
    
    sectionMIDIInfo=MIDIInfo::create();
    sectionMIDIInfo->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionMIDIInfo);
    
    sectionItemSettings=ItemSettings::create();
    sectionItemSettings->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionItemSettings);
    
    auto button = Button::create();
    button->loadTextureNormal("leftPanelTongueClosed.png");
    button->loadTexturePressed("leftPanelTongueClosed.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(arrowButtonSize);
    button->setPosition(Vec2(getContentSize().width*(1-PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE), getContentSize().height*(1-bitmapTopTransparencyPercentage)+(tongueTopTransparencyPercentage*arrowButtonSize.height)));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_HIDESHOW_PROPERTIES);
    
    button = Button::create();
    button->loadTextureNormal("newDefaultNew.png");
    button->loadTexturePressed("newActiveNew.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(buttonDim, buttonDim));
    button->setPosition(Vec2(buttonXPos, buttonYPos));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_NEW);
    
    buttonXPos+=(xpadding+buttonDim);
    
    button = Button::create();
    button->loadTextureNormal("saveDefaultNew.png");
    button->loadTexturePressed("saveActiveNew.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(buttonDim, buttonDim));
    button->setPosition(Vec2(buttonXPos, buttonYPos));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_SAVE);
    
    buttonXPos+=(xpadding+buttonDim);
    
    button = Button::create();
    button->loadTextureNormal("loadDefaultNew.png");
    button->loadTexturePressed("loadActiveNew.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(buttonDim, buttonDim));
    button->setPosition(Vec2(buttonXPos, buttonYPos));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_LOAD);
    
    buttonXPos+=(xpadding+buttonDim);
    
    button = Button::create();
    button->loadTextureNormal("moveActiveNew.png");
    button->loadTexturePressed("moveActiveNew.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(buttonDim, buttonDim));
    button->setPosition(Vec2(buttonXPos, buttonYPos));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,MAIN_BUTTON_EDIT);

    UpdateSubpanels();
}

void PropertiesPanel::UpdateEditButton(bool editMode)
{
    Button *b=dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_EDIT));
    if (editMode)
    {
        b->loadTextureNormal("moveDefaultNew.png");
        b->loadTexturePressed("moveDefaultNew.png");
    }
    else
    {
        b->loadTextureNormal("moveActiveNew.png");
        b->loadTexturePressed("moveActiveNew.png");
    }
}

void PropertiesPanel::OnHideShow()
{
    Button *b=dynamic_cast<Button*>(getChildByTag(MAIN_BUTTON_HIDESHOW_PROPERTIES));
    if (NULL==b) return;
    if (IsVisible())
    {
        b->loadTextureNormal("leftPanelTongueOpened.png");
        b->loadTexturePressed("leftPanelTongueOpened.png");
    }
    else
    {
        b->loadTextureNormal("leftPanelTongueClosed.png");
        b->loadTexturePressed("leftPanelTongueClosed.png");
    }
}

void PropertiesPanel::UpdateSubpanels()
{
    sectionOSCInfo->Update();
    //sectionMIDIDevices->Update();
    sectionMIDIInfo->Update();
    sectionItemSettings->Update();
    InitLayout();
}

void PropertiesPanel::UpdateOSCInfo()
{
    sectionOSCInfo->Update();
}

void PropertiesPanel::Update(SubjectSimple* subject, SCDFC_EVENTS event)
{
    if (NULL==subject)
        selectedItem=GetSelectedItem();
    else if (NULL!=dynamic_cast<ItemMultipad*>(subject))
        selectedItem=dynamic_cast<ItemMultipad*>(subject)->GetSelectedPad();
    else if (selectedItem==(ItemBase*)subject && event!=SCDFC_EVENTS_Move_Item)
        return;
    else
    {
        if (NULL!=selectedItem)
            selectedItem->Select(false);
        selectedItem=(ItemBase*)subject;
        selectedItem->Select(true);
    }
    
    if (!IsVisible())
    {
        if (event==SCDFC_EVENTS_Remove_Item)
            selectedItem=NULL;
        return;
    }
    LOGD ("Updating properties \n");
    switch (event)
    {
        case SCDFC_EVENTS_Move_Item:
            sectionMIDIInfo->UpdateVelocity();
            break;
        case SCDFC_EVENTS_Remove_Item:
            selectedItem=NULL;
        default:
            UpdateSubpanels();
            break;
    }
}

void PropertiesPanel::UpdateDevicesMenu()
{
    sectionMIDIInfo->UpdateDevicesMenu();
}

ItemBase *PropertiesPanel::GetSelectedItem()
{
    return selectedItem;
}

ScdfCtrl::MultiSender *PropertiesPanel::GetCurrentSender()
{
    if(NULL==selectedItem) return NULL;
    return selectedItem->GetControlUnit()->GetSender();
}
