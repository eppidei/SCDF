//
//  PropertiesPanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 21/12/14.
//
//

#include "SCDFCItems.h"
#include "PropertiesPanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "MainScene.h"
#include "MultiSender.h"
#include "SCDFCItems.h"
#include "LoadSavePanel.h"
//#include "PlatformInfo.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

#define VISIBILITY_CHECK \
        if (NULL==panel->GetSelectedItem()) \
        {   \
            if (isVisible()) \
                setVisible(false); \
            return; \
        } \
        if (!isVisible()) \
            setVisible(true); \

std::string GetDigitalFontPath()
{
    return "/Users/andreascuderi/Documents/SCDF/trunk/project/ScdfController/Resources/fonts/Open24.ttf";
    std::string s=std::string(getenv("HOME"))+"Documents/SCDF/trunk/project/ScdfController/Resources/fonts/Open24.ttf";
    return (FileUtils::getInstance()->getWritablePath()+"SCDF/trunk/project/ScdfController/Resources/fonts/Open24.ttf");
}

void CreateLabelWithBackground(SubpanelBase *subPanel, TextWithBackground **label, int labelID, cocos2d::Rect r, std::string labelText, std::string labelFont, int fontSize)
{
    (*label)=TextWithBackground::CreateText(PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW,r, labelText,labelFont,fontSize);
    if (-1!=labelID)
    {
        (*label)->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
        (*label)->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::LabelHeaderBackGround));
        (*label)->AddTouchCallback(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, subPanel));
    }
    else
    {
        (*label)->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::GRADIENT);
        (*label)->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientTop),Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientBottom));
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

void OSCInfo::InitChildrensVisibilityAndPos()
{
    bool hideItem=collapsed;
    HideElement(portLabel,hideItem);
    HideElement(oscPort,hideItem);
    HideElement(ipLabel,hideItem);
    HideElement(oscIP,hideItem);
    HideElement(oscTagLabel,hideItem);
    HideElement(oscTag,hideItem);
    SubpanelBase::InitChildrensVisibilityAndPos();
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
    CreateLabelWithBackground(this, &toggleLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "OSC", "Arial", 18);
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
    oscPort = TextInputWithBackground::CreateText(PROPERTIES_OSC_PORT, r, "No Port",GetDigitalFontPath(),20);
    addChild(oscPort,2);
    oscPort->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscPort->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    oscPort->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    
    //Create oscPort label
    CreateLabelWithBackground(this, &portLabel, -1, r, "UDP PORT", "Arial", 16);
    addChild(portLabel,3);
    
    //Create oscIP control
    oscIP = TextInputWithBackground::CreateText(PROPERTIES_OSC_IP, r, "No Ip",GetDigitalFontPath(),20);
    addChild(oscIP,4);
    oscIP->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscIP->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    oscIP->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));

    //Create oscIP label
    CreateLabelWithBackground(this, &ipLabel, -1, r, "IP ADDRESS", "Arial", 16);
    addChild(ipLabel,5);

    //Create OSCtag label
    CreateLabelWithBackground(this, &oscTagLabel, -1, r, "OSC TAG", "Arial", 16);
    addChild(oscTagLabel,6);
    
    //Create OSCtag label
    oscTag=TextWithBackground::CreateText(-1,r, "",GetDigitalFontPath(),16);
    oscTag->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    oscTag->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    addChild(oscTag,7);
    
    PositionElements();
    InitChildrensVisibilityAndPos();
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
    if (NULL==panel->GetCurrentSender()) return;
    if (panel->GetCurrentSender()->IsOscEnabled())
        oscToggle->loadTextureNormal("btnONSimple.png");
    else
        oscToggle->loadTextureNormal("btnOFFSimple.png");
}

void OSCInfo::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (panel->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
    UpdateOSCToggle();
    std::ostringstream os;
    os<<(panel->GetCurrentSender()->GetOscPort());
    oscPort->SetText(os.str());
    oscIP->SetText(panel->GetCurrentSender()->GetOscIp());
    oscTag->SetText(panel->GetCurrentSender()->GetOscTag());
//    InitChildrensVisibilityAndPos();
}

void OSCInfo::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    switch (widget->getTag())
    {
        case PROPERTIES_OSC_TOGGLE:
        {
            panel->GetCurrentSender()->SetOscEnabled(!panel->GetCurrentSender()->IsOscEnabled());
            UpdateOSCToggle();
            break;
        }
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

//MIDIDevices::MIDIDevices()
//{
//    devices=NULL;
//    devicesLabel=NULL;
//}

//void MIDIDevices::CreateControls()
//{
//    //Create dropDown label
//    cocos2d::Rect r(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT);
//    devicesLabel = TextWithBackground::CreateText(-1, r, "MIDI DEVICES","Arial",16);
//    addChild(devicesLabel);
//    devicesLabel->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::GRADIENT);
//    devicesLabel->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientTop),Colors::Instance()->GetUIColor(Colors::LabelBackGroundGradientBottom));
////    devicesLabel->ignoreContentAdaptWithSize(false);
////    devicesLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
////    devicesLabel->setAnchorPoint(Vec2(0,1));
////    devicesLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
////    devicesLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
////    devicesLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
//    
//    devices = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
//    addChild(devices);
//    devices->ignoreContentAdaptWithSize(false);
//    devices->setAnchorPoint(Vec2(0,1));
//    devices->SetCallback(dropDownCallback.get());
//    std::vector<DropDownMenuData> dropDownData;
//    dropDownData.push_back(DropDownMenuData("No MIDI connection",Colors::Instance()->GetUIColor(Colors::DropDownText)));
//    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
//        dropDownData.push_back(DropDownMenuData(Scdf::MidiOutConnection::GetOutputName(i),Colors::Instance()->GetUIColor(Colors::DropDownText)));
//    devices->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
//}

//void MIDIDevices::Update()
//{
//    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
//    if (panel->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
//    
//    devices->SetSelectedIndex(panel->GetCurrentControlUnit()->GetMidiOutIndex()+1);
//}

//void MIDIDevices::PositionElements()
//{
//    if (devicesLabel)
//        devicesLabel->setPosition(Vec2(0,getContentSize().height));
//    if (devices)
//        devices->setPosition(Vec2(0,getContentSize().height-devicesLabel->getContentSize().height));
//}

//void MIDIDevices::OnDropDownSelectionChange(DropDownMenu *menu)
//{
//    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
//    if (NULL==panel->GetCurrentControlUnit()) return;
//    int selectedIndex=menu->getCurSelectedIndex();
//    if (devices==menu)
//        panel->GetCurrentControlUnit()->SetMidiOutIndex(selectedIndex-1);
//}

void MIDIInfo::UpdateVelocity()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel) return; 
    velocity->SetSelectedIndex(panel->GetSelectedItem()->GetControlUnit()->GetValue());
    EnableElement(velocity, dynamic_cast<ItemSlider*>(panel->GetSelectedItem())==NULL && dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL);
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
    EnableElement(midiMessage, dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL);
    InitControlMenuData();

    channel->SetSelectedIndex(panel->GetCurrentSender()->GetMidiChannel());
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
    DoPosition(channelLabel, xOffset, yPos);
    DoPosition(channel, xOffset, yPos);
    DoPosition(velocityLabel, xOffset, yPos);
    DoPosition(velocity, xOffset, yPos);
}

void MIDIInfo::InitChildrensVisibilityAndPos()
{
    HideElement(devicesLabel,collapsed);
    HideElement(devices,collapsed);
    HideElement(midiMessageLabel,collapsed);
    HideElement(midiMessage,collapsed);
    HideElement(controlChangeLabel,collapsed);
    HideElement(controlChange,collapsed);
    HideElement(channelLabel,collapsed);
    HideElement(channel,collapsed);
    HideElement(velocityLabel,collapsed);
    HideElement(velocity,collapsed);
    SubpanelBase::InitChildrensVisibilityAndPos();
}

void MIDIInfo::InitControlMenuData()
{
    std::vector<DropDownMenuData> dropDownData;
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL)
    {
        if (midiMessage->getCurSelectedIndex()<2)
        {
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
            controlChangeLabel->SetText("MIDI NOTE");
        }
        else
        {
            for (int i=0;i<120;++i)
            {
                std::ostringstream os;
                os<<"CC: "<<i;
                dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
            }
            controlChangeLabel->SetText("CONTROL CHANGE");
        }
    }
    else
    {
        for (int i=0;i<11;++i)
        {
            std::ostringstream os;
            os<<i-1;
            dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
        }
        controlChangeLabel->SetText("OCTAVE");
    }
    controlChange->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    int value=0;
    if (panel->GetSelectedItem()==NULL) return;
    if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL)
        value=panel->GetCurrentSender()->GetMidiControl();
    else
        value=dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())->GetCurrentOctave();
    controlChange->SetSelectedIndex(value);
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
        panel->GetCurrentSender()->SetMidiMessageType(msg);
        InitControlMenuData();
    }
    else if (menu==controlChange)
    {
        if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL)
            panel->GetCurrentSender()->SetMidiControl(selectedIndex);
        else
            dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())->SetCurrentOctave(selectedIndex);
    }
    else if (menu==channel)
        panel->GetCurrentSender()->SetMidiChannel(selectedIndex);
    else if (menu==velocity)
        panel->GetSelectedItem()->GetControlUnit()->SetMax(selectedIndex);
    	// this is used for the button, and the button
    	// sends the max value when pressed
    else if (menu==devices)
        panel->GetCurrentSender()->SetMidiOutIndex(selectedIndex-1);
    panel->UpdateOSCInfo();
    
    panel->UpdateOSCInfo();
}

MIDIInfo::MIDIInfo()
{
    devices=NULL;
    devicesLabel=NULL;
    midiMessage=controlChange=channel=velocity=NULL;
    midiMessageLabel=controlChangeLabel=channelLabel=velocityLabel=midiLabel=NULL;
}

void MIDIInfo::CreateControls()
{
    cocos2d::Rect r(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT);
    
    //Create header
    CreateLabelWithBackground(this, &midiLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "MIDI", "Arial", 18);
    addChild(midiLabel);
    
    r.size.width-=GetYPadding();

    //Create device label
    CreateLabelWithBackground(this, &devicesLabel, -1, r, "MIDI DEVICES", "Arial", 16);
    addChild(devicesLabel);
    
    //Create device dropDown
    devices = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(devices);
    std::vector<DropDownMenuData> dropDownData;
    dropDownData.push_back(DropDownMenuData("No MIDI connection",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(DropDownMenuData(Scdf::MidiOutConnection::GetOutputName(i),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    devices->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create midiMessage label
    CreateLabelWithBackground(this, &midiMessageLabel, -1, r, "MIDI MESSAGE", "Arial", 16);
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
    dropDownData.clear();
    for (int i=0;i<MidiMessageString.size();++i)
        dropDownData.push_back(DropDownMenuData(MidiMessageString[i],Colors::Instance()->GetUIColor(Colors::DropDownText)));
    midiMessage->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create ControlChange label
    CreateLabelWithBackground(this, &controlChangeLabel, -1, r, "CONTROL CHANGE", "Arial", 16);
    addChild(controlChangeLabel);

    //Create ControlChange dropDown
    controlChange = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(controlChange);
    InitControlMenuData();
    
    //Create channel label
    CreateLabelWithBackground(this, &channelLabel, -1, r, "CHANNEL", "Arial", 16);
    addChild(channelLabel);

    //Create channel dropDown
    channel = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(channel);
    dropDownData.clear();
    for (int i=0;i<16;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    channel->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);

    //Create velocity label
    CreateLabelWithBackground(this, &velocityLabel, -1, r, "VELOCITY", "Arial", 16);
    addChild(velocityLabel);
    
    //Create velocity dropDown
    velocity = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(velocity);
    dropDownData.clear();
    for (int i=0;i<128;++i)
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
    
}

void ItemSettings::InitChildrensVisibilityAndPos()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    HideElement(nameLabel,collapsed);
    HideElement(name,collapsed);
    HideElement(colorLabel,collapsed);
    HideElement(color,collapsed);
    HideElement(controlLabel,collapsed);
    HideElement(modes,collapsed);
    
    bool forceHide=false;
    if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())!=NULL)
        forceHide=true;
    
    HideElement(sizeLabel,collapsed||forceHide);
    HideElement(h_plus,collapsed||forceHide);
    HideElement(sizeText,collapsed||forceHide);
    HideElement(h_minus,collapsed||forceHide);
    
    forceHide=false;
    if (dynamic_cast<ItemPad*>(panel->GetSelectedItem())!=NULL ||
        dynamic_cast<ItemKnob*>(panel->GetSelectedItem())!=NULL)
        forceHide=true;
    
    HideElement(orientation,collapsed||forceHide);
    HideElement(orientLabel,collapsed||forceHide);
    SubpanelBase::InitChildrensVisibilityAndPos();
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
    DoPosition(color, xOffset, yPos);
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
    CreateLabelWithBackground(this, &settingsLabel, PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW, r, "ITEM SETTINGS", "Arial", 18);
    addChild(settingsLabel,1);
    
    r.size.width-=GetYPadding();

    //Create name label
    CreateLabelWithBackground(this, &nameLabel, -1, r, "NAME", "Arial", 16);
    addChild(nameLabel,2);
    
    //Create name control
    name = TextInputWithBackground::CreateText(PROPERTIES_ITEM_NAME, r, "No Name",GetDigitalFontPath(),20);
    addChild(name,3);
    name->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    name->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    name->AddEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    
    //Create color label
    CreateLabelWithBackground(this, &colorLabel, -1, r, "COLOR", "Arial", 16);
    addChild(colorLabel,4);
    
    //Create color control
    color = DropDownMenu::CreateMenu<DropDownColorMenu>(r.size, dropDownCallback.get());
    addChild(color,5,PROPERTIES_ITEM_COLOR);
    color->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::SubpanelGenericItem));
    
    std::vector<DropDownMenuData> dropDownData;
    for (int i=0;i<Colors::Instance()->CountItemsColor();++i)
        dropDownData.push_back(DropDownMenuData("",Colors::Instance()->GetItemsColor((Colors::ItemsColorsId)i)));
    
    color->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //create control modes label
    CreateLabelWithBackground(this, &controlLabel, -1, r, "CONTROL MODE", "Arial", 16);
    addChild(controlLabel,6);
    
    //create control modes control

    const float numButtons=5.0;
    const float numbButtonsPerRow=3.0;
    const float numButtonsPerColumn=numButtons-numbButtonsPerRow;
    const float xoffset=6.0;
    const float yoffset=8.0;
    float bSize=(r.size.width-((numbButtonsPerRow+1.0)*xoffset))/numbButtonsPerRow;
    cocos2d::Size buttonSize(bSize, bSize);
    cocos2d::Rect rToolbar(0,0,r.size.width,buttonSize.height*(numButtonsPerColumn)+(numButtonsPerColumn+1.0)*yoffset);
    modes = Toolbar::CreateToolbar(rToolbar, xoffset, yoffset);
    addChild(modes,7);
    color->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::SubpanelGenericItem));
    std::vector<std::string> images;
    images.push_back("CloseNormal.png");
    images.push_back("CloseSelected.png");
    modes->AddButton(PROPERTIES_CONTROLMODE_WIRE, buttonSize, images, CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    modes->AddButton(PROPERTIES_CONTROLMODE_BLOW, buttonSize, images, CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    modes->AddButton(PROPERTIES_CONTROLMODE_PIPPO, buttonSize, images, CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    modes->AddButton(PROPERTIES_CONTROLMODE_PLUTO, buttonSize, images, CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    modes->AddButton(PROPERTIES_CONTROLMODE_PAPERINO, buttonSize, images, CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));

    //Create group label
    CreateLabelWithBackground(this, &groupLabel, -1, r, "GROUP", "Arial", 16);
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
    masterButton->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    masterButton->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    masterButton->setTouchEnabled(true);
    masterButton->setContentSize(cocos2d::Size(r.size.width,r.size.height));
    masterButton->setAnchorPoint(Vec2(0,1));
    addChild(masterButton,10,PROPERTIES_ITEMSETTINGS_MASTER);
    masterButton->ignoreContentAdaptWithSize(false);
    
    //Create orient label
    CreateLabelWithBackground(this, &orientLabel, -1, r, "ORIENT", "Arial", 16);
    addChild(orientLabel,11);
    
    //Create orient dropDown
    orientation = DropDownMenu::CreateMenu<DropDownMenu>(r.size, dropDownCallback.get());
    addChild(orientation,12);
    dropDownData.clear();
    dropDownData.push_back(DropDownMenuData("Vertical",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    dropDownData.push_back(DropDownMenuData("Horizontal",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    orientation->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create size label
    CreateLabelWithBackground(this, &sizeLabel, -1, r, "SIZE", "Arial", 16);
    addChild(sizeLabel,13);
    
    //Create size control
    h_minus = ui::Button::create();
    addChild(h_minus, 15, PROPERTIES_ITEM_HEIGHT_MINUS);
    h_minus->setTouchEnabled(true);
    h_minus->ignoreContentAdaptWithSize(false);
    h_minus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    h_minus->setAnchorPoint(Vec2(0,1));
    h_minus->setContentSize(cocos2d::Size(r.size.height, r.size.height));
    h_minus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    
    CreateLabelWithBackground(this, &sizeText, -1, r, "", GetDigitalFontPath(), 18);
    sizeText->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    sizeText->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
    addChild(sizeText,14);
    sizeText->SetAlignement(TextHAlignment::CENTER, TextVAlignment::CENTER);
    
    h_plus = ui::Button::create();
    addChild(h_plus, 16, PROPERTIES_ITEM_HEIGHT_PLUS);
    h_plus->ignoreContentAdaptWithSize(false);
    h_plus->setTouchEnabled(true);
    h_plus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    h_plus->setAnchorPoint(Vec2(0,1));
    h_plus->setContentSize(cocos2d::Size(r.size.height, r.size.height));
    h_plus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
}

void ItemSettings::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    VISIBILITY_CHECK
    
    ItemBase *item=panel->GetSelectedItem();
    if(NULL==item) return;
    std::ostringstream os;
    char str[256];
    sprintf(str,"x %d",item->GetLayoutManager()->GetMagValue());
    sizeText->SetText(str);
    color->SetSelectedIndex(item->GetColor());
    name->SetText(item->GetName());
    modes->CheckButton((int)(item->GetControlUnit()->GetType())+PROPERTIES_CONTROLMODE_BASE);
    int orientationIndex=1;
    if (item->GetLayoutManager()->IsVertical())
        orientationIndex=0;
    orientation->SetSelectedIndex(orientationIndex);
    group->SetSelectedIndex(item->GetGroupID()+1);
    if (item->IsMaster())
        masterButton->loadTextureNormal("CloseSelected.png");
    else
        masterButton->loadTextureNormal("CloseNormal.png");
    InitChildrensVisibilityAndPos();
    
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
}

void ItemSettings::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
    
    switch (widget->getTag())
    {
        case PROPERTIES_ITEM_HEIGHT_MINUS:
            panel->GetSelectedItem()->GetLayoutManager()->ZoomMinus();
            break;
        case PROPERTIES_ITEM_HEIGHT_PLUS:
            panel->GetSelectedItem()->GetLayoutManager()->ZoomPlus();
            break;
        case PROPERTIES_ITEMSETTINGS_MASTER:
            panel->GetSelectedItem()->SetMaster(!panel->GetSelectedItem()->IsMaster());
            break;
        case PROPERTIES_CONTROLMODE_WIRE:
        case PROPERTIES_CONTROLMODE_BLOW:
        case PROPERTIES_CONTROLMODE_PIPPO:
        case PROPERTIES_CONTROLMODE_PLUTO:
        case PROPERTIES_CONTROLMODE_PAPERINO:
            panel->GetSelectedItem()->ChangeControlUnit((ControlUnit::Type)(widget->getTag()-PROPERTIES_CONTROLMODE_BASE));
        default:
            break;
    }
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
    setBackGroundImage("leftPanel.png");
    const int rightPadding=60;
    selectedItem=NULL;
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionOSCInfo);
    
//    sectionMIDIDevices=MIDIDevices::create();
//    sectionMIDIDevices->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width/2));
//    scrollView->addChild(sectionMIDIDevices);
    
    sectionMIDIInfo=MIDIInfo::create();
    sectionMIDIInfo->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionMIDIInfo);
    
    sectionItemSettings=ItemSettings::create();
    sectionItemSettings->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionItemSettings);
    
    
    auto button = Button::create();
    button->loadTextureNormal("btnOFF.png");
    button->loadTexturePressed("btnON.png");
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(cocos2d::Size(30, 60));
    button->setPosition(Vec2(getContentSize().width-80, getContentSize().height-28));
    button->addTouchEventListener(CC_CALLBACK_2(MainScene::touchEvent, parent));
    addChild(button,6,TOOLBAR_BUTTON_HIDESHOW_PROPERTIES);
    UpdateSubpanels();
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
        selectedItem=(ItemBase*)subject;
    
    if (!IsVisible()) return;
    LOGD ("Updating properties \n");
    switch (event)
    {
//        case SCDFC_EVENTS_Add_Item_Slider:
//            sectionMIDIInfo->InitForSlider();
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

ItemBase *PropertiesPanel::GetSelectedItem()
{
    return selectedItem;
}

ScdfCtrl::MultiSender *PropertiesPanel::GetCurrentSender()
{
    if(NULL==selectedItem) return NULL;
    return selectedItem->GetControlUnit()->GetSender();
}
