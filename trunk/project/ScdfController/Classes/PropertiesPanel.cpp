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


void OSCInfo::InitChildrensVisibilityAndPos()
{
    bool hideItem=!oscToggle->getSelectedState();
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
    toggleLabel=portLabel=ipLabel=oscTagLabel=oscTag=NULL;
    oscToggle=NULL;
    oscPort=oscIP=NULL;
}

void OSCInfo::CreateControls()
{
    //Create toggle
    oscToggle=CheckBox::create();
    oscToggle->loadTextures("ToggleOn.png",
                            "ToggleOff.png",
                            "ToggleOff.png",
                            "ToggleOff.png",
                            "ToggleOn.png");

    
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(cocos2d::Size(SUBPANEL_ITEM_HEIGHT,SUBPANEL_ITEM_HEIGHT));
    oscToggle->setAnchorPoint(Vec2(0,1));
    oscToggle->setSelectedState(false);
    oscToggle->addEventListener(CC_CALLBACK_2(SubpanelBase::CheckBoxEventCallback, this));
    addChild(oscToggle,0,PROPERTIES_OSC_TOGGLE);
    oscToggle->ignoreContentAdaptWithSize(false);
    
    //Create toggle label
    toggleLabel = Text::create("ENABLE OSC","Arial",16);
    toggleLabel->ignoreContentAdaptWithSize(false);
    toggleLabel->setAnchorPoint(Vec2(0,1));
    toggleLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    toggleLabel->setContentSize(cocos2d::Size(getContentSize().width/2,SUBPANEL_ITEM_HEIGHT));
    toggleLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    addChild(toggleLabel);
    
    //Create oscPort control
    oscPort=TextField::create();
    oscPort->ignoreContentAdaptWithSize(false);
    oscPort->setPlaceHolder("No Port");
    oscPort->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscPort->setAnchorPoint(Vec2(0,1));
    oscPort->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscPort->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscPort->setFontSize(20);
    oscPort->setTouchEnabled(true);
    oscPort->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    oscPort->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    addChild(oscPort, 0, PROPERTIES_OSC_PORT);
    
    //Create oscPort label
    portLabel = Text::create("UDP PORT","Arial",16);
    portLabel->ignoreContentAdaptWithSize(false);
    portLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    portLabel->setAnchorPoint(Vec2(0,1));
    portLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    portLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    portLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    addChild(portLabel);
    
    //Create oscIP control
    oscIP=TextField::create();
    oscIP->setPlaceHolder("No Ip");
    oscIP->ignoreContentAdaptWithSize(false);
    oscIP->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscIP->setAnchorPoint(Vec2(0,1));
    oscIP->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscIP->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscIP->setTouchEnabled(true);
    oscIP->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    oscIP->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    addChild(oscIP, 0 , PROPERTIES_OSC_IP);
    oscIP->setFontName("Arial");
    oscIP->setFontSize(20);
    
    
    //Create oscIP label
    ipLabel = Text::create("IP ADDRESS","Arial",16);
    ipLabel->ignoreContentAdaptWithSize(false);
    ipLabel->setAnchorPoint(Vec2(0,1));
    ipLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    ipLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    ipLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    ipLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    addChild(ipLabel);
    
    //Create OSCtag label
    oscTagLabel = Text::create("OSC TAG","Arial",16);
    oscTagLabel->ignoreContentAdaptWithSize(false);
    oscTagLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTagLabel->setAnchorPoint(Vec2(0,1));
    oscTagLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTagLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscTagLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    addChild(oscTagLabel);
    
    //Create OSCtag label
    oscTag = Text::create("","Arial",16);
    oscTag->setString("No Tag");
    oscTag->ignoreContentAdaptWithSize(false);
    oscTag->setAnchorPoint(Vec2(0,1));
    oscTag->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTag->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTag->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscTag->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    addChild(oscTag);
    PositionElements();
    InitChildrensVisibilityAndPos();
}

void OSCInfo::PositionElements()
{
    float yPos=getContentSize().height;
    
    if (toggleLabel)
    {
        toggleLabel->setPosition(Vec2(0,getContentSize().height));
        yPos-=toggleLabel->getContentSize().height;
    }
    
    if (oscToggle)
        oscToggle->setPosition(Vec2(getContentSize().width/2+getContentSize().width/4-oscToggle->getContentSize().width/2,getContentSize().height-SUBPANEL_ITEM_HEIGHT/2+oscToggle->getContentSize().height/2));
    
    if (portLabel && portLabel->isVisible())
    {
        portLabel->setPosition(Vec2(0,yPos));
        yPos-=portLabel->getContentSize().height;
    }
    if (oscPort && oscPort->isVisible())
    {
        oscPort->setPosition(Vec2(0,yPos));
        yPos-=oscPort->getContentSize().height;
    }

    if (ipLabel && ipLabel->isVisible())
    {
        ipLabel->setPosition(Vec2(0,yPos));
        yPos-=ipLabel->getContentSize().height;
    }
    if (oscIP && oscIP->isVisible())
    {
        oscIP->setPosition(Vec2(0,yPos));
        yPos-=oscIP->getContentSize().height;
    }
    
    if (oscTagLabel && oscTagLabel->isVisible())
    {
        oscTagLabel->setPosition(Vec2(0,yPos));
        yPos-=oscTagLabel->getContentSize().height;
    }
    
    if (oscTag && oscTag->isVisible())
        oscTag->setPosition(Vec2(0,yPos));
}

void OSCInfo::OnCheckEvent(CheckBox *widget, bool selected)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    switch (widget->getTag())
    {
        case PROPERTIES_OSC_TOGGLE:
            panel->GetCurrentSender()->SetOscEnabled(selected);
            break;
        default:
            break;
    }
    InitChildrensVisibilityAndPos();
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

MIDIDevices::MIDIDevices()
{
    devices=NULL;
    devicesLabel=NULL;
}

void MIDIDevices::CreateControls()
{
    //Create dropDown label
    devicesLabel = Text::create("MIDI DEVICES","Arial",16);
    addChild(devicesLabel);
    devicesLabel->ignoreContentAdaptWithSize(false);
    devicesLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    devicesLabel->setAnchorPoint(Vec2(0,1));
    devicesLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    devicesLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    devicesLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    
    devices = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(devices);
    devices->ignoreContentAdaptWithSize(false);
    devices->setAnchorPoint(Vec2(0,1));
    devices->SetCallback(dropDownCallback.get());
    std::vector<DropDownMenuData> dropDownData;
    dropDownData.push_back(DropDownMenuData("No MIDI connection",Colors::Instance()->GetUIColor(Colors::DropDownText)));
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(DropDownMenuData(Scdf::MidiOutConnection::GetOutputName(i),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    devices->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
}

void MIDIDevices::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (panel->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
    devices->SetSelectedIndex(panel->GetCurrentSender()->GetMidiOutIndex()+1);
}

void MIDIDevices::PositionElements()
{
    if (devicesLabel)
        devicesLabel->setPosition(Vec2(0,getContentSize().height));
    if (devices)
        devices->setPosition(Vec2(0,getContentSize().height-devicesLabel->getContentSize().height));
}

void MIDIDevices::OnDropDownSelectionChange(DropDownMenu *menu)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetCurrentSender()) return;
    int selectedIndex=menu->getCurSelectedIndex();
    if (devices==menu)
        panel->GetCurrentSender()->SetMidiOutIndex(selectedIndex-1);
}

void OSCInfo::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (panel->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
    oscToggle->setSelectedState(panel->GetCurrentSender()->IsOscEnabled());
    std::ostringstream os;
    os<<(panel->GetCurrentSender()->GetOscPort());
    oscPort->setText(os.str());
    oscIP->setText(panel->GetCurrentSender()->GetOscIp());
    oscTag->setString(panel->GetCurrentSender()->GetOscTag());
    InitChildrensVisibilityAndPos();
}

void MIDIInfo::UpdateVelocity()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel) return; 
    velocity->SetSelectedIndex(panel->GetSelectedItem()->GetValue());
    velocity->setEnabled(dynamic_cast<ItemSlider*>(panel->GetSelectedItem())==NULL && dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())==NULL);
}

void MIDIInfo::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    VISIBILITY_CHECK
    
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
    InitControlMenuValue();
    if (dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())!=NULL)
    {
        midiMessage->setEnabled(false);
        controlChange->SetSelectedIndex(dynamic_cast<ItemKeyboard*>(panel->GetSelectedItem())->GetCurrentOctave());
    }
    else
    {
        midiMessage->setEnabled(true);
        controlChange->SetSelectedIndex(panel->GetCurrentSender()->GetMidiControl());
        channel->SetSelectedIndex(panel->GetCurrentSender()->GetMidiChannel());
    }
    UpdateVelocity();
}

void MIDIInfo::PositionElements()
{
    if (midiMessageLabel)
        midiMessageLabel->setPosition(Vec2(0,getContentSize().height));
    if (midiMessage)
        midiMessage->setPosition(Vec2(0,midiMessageLabel->getPositionY()-midiMessageLabel->getContentSize().height));
    if (controlChangeLabel)
        controlChangeLabel->setPosition(Vec2(0,midiMessage->getPositionY()-midiMessage->getContentSize().height));
    if (controlChange)
        controlChange->setPosition(Vec2(0,controlChangeLabel->getPositionY()-controlChangeLabel->getContentSize().height));
    if (channelLabel)
        channelLabel->setPosition(Vec2(0,controlChange->getPositionY()-controlChange->getContentSize().height));
    if (channel)
        channel->setPosition(Vec2(0,channelLabel->getPositionY()-channelLabel->getContentSize().height));
    if (velocityLabel)
        velocityLabel->setPosition(Vec2(0,channel->getPositionY()-channel->getContentSize().height));
    if (velocity)
        velocity->setPosition(Vec2(0,velocityLabel->getPositionY()-velocityLabel->getContentSize().height));
}

void MIDIInfo::InitControlMenuValue()
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
            controlChangeLabel->setString("MIDI NOTE");
        }
        else
        {
            for (int i=0;i<120;++i)
            {
                std::ostringstream os;
                os<<"CC: "<<i;
                dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
            }
            controlChangeLabel->setString("CONTROL CHANGE");
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
        controlChangeLabel->setString("OCTAVE");
    }
    controlChange->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
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
        InitControlMenuValue();
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
    panel->UpdateOSCInfo();
    
}

MIDIInfo::MIDIInfo()
{
    midiMessage=controlChange=channel=velocity=NULL;
    midiMessageLabel=controlChangeLabel=channelLabel=velocityLabel=NULL;
}

void MIDIInfo::CreateControls()
{
    std::vector<std::string> MidiMessageString;
    MidiMessageString.push_back("Note On");
    MidiMessageString.push_back("Note Off");
    MidiMessageString.push_back("Aftertouch");
    MidiMessageString.push_back("Control Change");
    MidiMessageString.push_back("Program Change");
    MidiMessageString.push_back("PolyKeyPressure");
    MidiMessageString.push_back("Pitch Bend");
    
    //Create midiMessage label
    midiMessageLabel = Text::create("MIDI MESSAGE","Arial",16);
    addChild(midiMessageLabel);
    midiMessageLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    midiMessageLabel->ignoreContentAdaptWithSize(false);
    midiMessageLabel->setAnchorPoint(Vec2(0,1));
    midiMessageLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    midiMessageLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    midiMessageLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));

    
    //Create dropDown midiMessage
    midiMessage = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(midiMessage);
    midiMessage->ignoreContentAdaptWithSize(false);
    midiMessage->setAnchorPoint(Vec2(0,1));
    midiMessage->SetCallback(dropDownCallback.get());
    std::vector<DropDownMenuData> dropDownData;
    for (int i=0;i<MidiMessageString.size();++i)
        dropDownData.push_back(DropDownMenuData(MidiMessageString[i],Colors::Instance()->GetUIColor(Colors::DropDownText)));
    midiMessage->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create dropDown label
    controlChangeLabel = Text::create("CONTROL CHANGE","Arial",16);
    addChild(controlChangeLabel);
    controlChangeLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    controlChangeLabel->ignoreContentAdaptWithSize(false);
    controlChangeLabel->setAnchorPoint(Vec2(0,1));
    controlChangeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    controlChangeLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    controlChangeLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));


    //Create dropDown
    controlChange = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(controlChange);
    controlChange->ignoreContentAdaptWithSize(false);
    controlChange->setAnchorPoint(Vec2(0,1));
    controlChange->SetCallback(dropDownCallback.get());
    dropDownData.clear();
    
    InitControlMenuValue();
    //Create dropDown label
    channelLabel = Text::create("CHANNEL","Arial",16);
    addChild(channelLabel);
    channelLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    channelLabel->ignoreContentAdaptWithSize(false);
    channelLabel->setAnchorPoint(Vec2(0,1));
    channelLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    channelLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    channelLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));


    //Create dropDown
    channel = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(channel);
    channel->ignoreContentAdaptWithSize(false);
    channel->setAnchorPoint(Vec2(0,1));
    channel->SetCallback(dropDownCallback.get());
    dropDownData.clear();
    for (int i=0;i<16;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(DropDownMenuData(os.str(),Colors::Instance()->GetUIColor(Colors::DropDownText)));
    }
    channel->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);

    //Create dropDown label
    velocityLabel = Text::create("VELOCITY","Arial",16);
    addChild(velocityLabel);
    velocityLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    velocityLabel->ignoreContentAdaptWithSize(false);
    velocityLabel->setAnchorPoint(Vec2(0,1));
    velocityLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    velocityLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    velocityLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));


    //Create dropDown
    velocity = DropDownMenu::CreateMenu<DropDownMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(velocity);
    velocity->ignoreContentAdaptWithSize(false);
    velocity->setAnchorPoint(Vec2(0,1));
    velocity->SetCallback(dropDownCallback.get());
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
    color=NULL;
    name=NULL;
    sizeLabel=colorLabel=nameLabel=NULL;
}

void ItemSettings::PositionElements()
{
    if (nameLabel)
        nameLabel->setPosition(Vec2(0,getContentSize().height));
    if (name)
        name->setPosition(Vec2(0,nameLabel->getPositionY()-nameLabel->getContentSize().height));
    if (colorLabel)
        colorLabel->setPosition(Vec2(0,name->getPositionY()-name->getContentSize().height));
    if (color)
        color->setPosition(Vec2(0,colorLabel->getPositionY()-colorLabel->getContentSize().height));

    if (sizeLabel)
        sizeLabel->setPosition(Vec2(0,color->getPositionY()-color->getContentSize().height));
    if (h_plus)
    {
        h_plus->setPosition(Vec2(getContentSize().width/2.0-h_plus->getContentSize().width/2.0,sizeLabel->getPositionY()-sizeLabel->getContentSize().height));
        sizeText->setPosition(Vec2(getContentSize().width/2.0-sizeText->getContentSize().width/2.0,h_plus->getPositionY()-h_plus->getContentSize().height));
        w_minus->setPosition(Vec2(sizeText->getPositionX()-w_minus->getContentSize().width,h_plus->getPositionY()-h_plus->getContentSize().height));
        w_plus->setPosition(Vec2(sizeText->getPositionX()+sizeText->getContentSize().width,h_plus->getPositionY()-h_plus->getContentSize().height));
        h_minus->setPosition(Vec2(getContentSize().width/2.0-h_minus->getContentSize().width/2.0,w_plus->getPositionY()-w_plus->getContentSize().height));
    }
}

void ItemSettings::CreateControls()
{
    sizeLabel = Text::create("ITEM SIZE","Arial",16);
    addChild(sizeLabel);
    sizeLabel->ignoreContentAdaptWithSize(false);
    sizeLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    sizeLabel->setAnchorPoint(Vec2(0,1));
    sizeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    sizeLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    sizeLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    
    colorLabel = Text::create("ITEM COLOR","Arial",16);
    addChild(colorLabel);
    colorLabel->ignoreContentAdaptWithSize(false);
    colorLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    colorLabel->setAnchorPoint(Vec2(0,1));
    colorLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    colorLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    colorLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    
    nameLabel = Text::create("ITEM NAME","Arial",16);
    addChild(nameLabel);
    nameLabel->ignoreContentAdaptWithSize(false);
    nameLabel->setAnchorPoint(Vec2(0,1));
    nameLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    nameLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    nameLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    nameLabel->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    
    name=TextField::create();
    addChild(name, 0, PROPERTIES_ITEM_NAME);
    name->ignoreContentAdaptWithSize(false);
    name->setPlaceHolder("No name");
    name->setTextVerticalAlignment(TextVAlignment::CENTER);
    name->setTextHorizontalAlignment(TextHAlignment::CENTER);
    name->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    name->setAnchorPoint(Vec2(0,1));
    name->setFontSize(20);
    name->setTouchEnabled(true);
    name->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
    name->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    
    color = DropDownMenu::CreateMenu<DropDownColorMenu>(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(color,0,PROPERTIES_ITEM_COLOR);
    color->ignoreContentAdaptWithSize(false);
    color->setAnchorPoint(Vec2(0,1));
    color->SetCallback(dropDownCallback.get());
    
    std::vector<DropDownMenuData> dropDownData;
    for (int i=0;i<Colors::Instance()->CountItemsColor();++i)
        dropDownData.push_back(DropDownMenuData("",Colors::Instance()->GetItemsColor((Colors::ItemsColorsId)i)));
    
    color->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);

    
    h_minus = ui::Button::create();
    addChild(h_minus, 0, PROPERTIES_ITEM_HEIGHT_MINUS);
    h_minus->setTouchEnabled(true);
    h_minus->ignoreContentAdaptWithSize(false);
    h_minus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    h_minus->setAnchorPoint(Vec2(0,1));
    h_minus->setContentSize(cocos2d::Size(SUBPANEL_ITEM_HEIGHT, SUBPANEL_ITEM_HEIGHT));
    h_minus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    
    h_plus = ui::Button::create();
    addChild(h_plus, 0, PROPERTIES_ITEM_HEIGHT_PLUS);
    h_plus->ignoreContentAdaptWithSize(false);
    h_plus->setTouchEnabled(true);
    h_plus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    h_plus->setAnchorPoint(Vec2(0,1));
    h_plus->setContentSize(cocos2d::Size(SUBPANEL_ITEM_HEIGHT, SUBPANEL_ITEM_HEIGHT));
    h_plus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));

    w_minus = ui::Button::create();
    addChild(w_minus, 0, PROPERTIES_ITEM_WIDTH_MINUS);
    w_minus->setTouchEnabled(true);
    w_minus->ignoreContentAdaptWithSize(false);
    w_minus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    w_minus->setAnchorPoint(Vec2(0,1));
    w_minus->setContentSize(cocos2d::Size(SUBPANEL_ITEM_HEIGHT, SUBPANEL_ITEM_HEIGHT));
    w_minus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    
    w_plus = ui::Button::create();
    addChild(w_plus, 0, PROPERTIES_ITEM_WIDTH_PLUS);
    w_plus->ignoreContentAdaptWithSize(false);
    w_plus->setTouchEnabled(true);
    w_plus->loadTextures("CloseNormal.png", "CloseSelected.png", "");
    w_plus->setAnchorPoint(Vec2(0,1));
    w_plus->setContentSize(cocos2d::Size(SUBPANEL_ITEM_HEIGHT, SUBPANEL_ITEM_HEIGHT));
    w_plus->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    
    sizeText = Text::create("","Arial",16);
    addChild(sizeText);
    sizeText->ignoreContentAdaptWithSize(false);
    sizeText->setAnchorPoint(Vec2(0,1));
    sizeText->setTextVerticalAlignment(TextVAlignment::CENTER);
    sizeText->setTextHorizontalAlignment(TextHAlignment::CENTER);
    sizeText->setContentSize(cocos2d::Size(getContentSize().width/3,SUBPANEL_ITEM_HEIGHT));
    sizeText->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
}

void ItemSettings::Update()
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    VISIBILITY_CHECK
    
    ItemBase *item=panel->GetSelectedItem();
    if(NULL==item) return;
    std::ostringstream os;
    char str[256];
    sprintf(str,"(%d,%d)",(int)(item->GetStaticBaseSize().width+item->GetInflateValue(false)),(int)(item->GetStaticBaseSize().height+item->GetInflateValue(true)));
    sizeText->setString(str);
    color->SetSelectedIndex(panel->GetSelectedItem()->GetColor());
    name->setText(panel->GetSelectedItem()->GetName());
    
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
}

void ItemSettings::OnTouchEventBegan(cocos2d::Node *widget)
{
    PropertiesPanel *panel=dynamic_cast<PropertiesPanel*>(GetParent());
    if (NULL==panel->GetSelectedItem()) return;
    
    switch (widget->getTag())
    {
        case PROPERTIES_ITEM_HEIGHT_MINUS:
            panel->GetSelectedItem()->DecrementInflate(true);
            break;
        case PROPERTIES_ITEM_HEIGHT_PLUS:
            panel->GetSelectedItem()->IncrementInflate(true);
            break;
        case PROPERTIES_ITEM_WIDTH_MINUS:
            panel->GetSelectedItem()->DecrementInflate(false);
            break;
        case PROPERTIES_ITEM_WIDTH_PLUS:
            panel->GetSelectedItem()->IncrementInflate(false);
            break;
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
//            cocos2d::Rect rr(0, 0,     getContentSize().width/*, getBackGroundImageTextureSize().width-0*/,     /*getBackGroundImageTextureSize()*/getContentSize().height-0);
//    auto jacket = Sprite::create("leftPanel.png",rr);
//    addChild(jacket);
    setBackGroundImage("leftPanel.png");
    const int rightPadding=60;
    selectedItem=NULL;
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width));
    scrollView->addChild(sectionOSCInfo);
    
    sectionMIDIDevices=MIDIDevices::create();
    sectionMIDIDevices->InitWithContent(this, cocos2d::Size(scrollView->getContentSize().width,scrollView->getContentSize().width/2));
    scrollView->addChild(sectionMIDIDevices);
    
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
//    cocos2d::Rect rr(0, 0, getContentSize().width, getContentSize().height);
//    auto backGroundImage = Sprite::create("leftPanel.png");
//    addChild(backGroundImage);
//    cocos2d::Size s=backGroundImage->getContentSize();
//    backGroundImage->setAnchorPoint(Vec2(0,1));
//    backGroundImage->setScale(getContentSize().width/s.width, getInnerContainerSize().height/s.height);
//    backGroundImage->setPosition(0,getContentSize().height);
}

void PropertiesPanel::UpdateSubpanels()
{
    sectionOSCInfo->Update();
    sectionMIDIDevices->Update();
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
    
    if (getPositionX()<0) return;
    printf ("Updating properties \n");
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
    return selectedItem->GetSender();
}
