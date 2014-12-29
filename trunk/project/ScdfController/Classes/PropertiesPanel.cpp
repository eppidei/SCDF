//
//  PropertiesPanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 21/12/14.
//
//

#include "PropertiesPanel.h"
#include "SCDFCWorkingPanel.h"
#include "SCDFCScrollView.h"
#include "MainScene.h"
#include "ControlUnit.h"
#include "SCDFCItems.h"

using namespace SCDFC;
using namespace cocos2d;
using namespace ui;

void DropDownMenuCallbackPanel::OnSizeChanged(float oldSize, float newSize)
{
    parent->InitLayout();
}

void DropDownMenuCallbackPanel::OnSelectItem(DropDownMenu *menu)
{
    parent->OnSelectedDropDownItem(menu);
}

SubpanelBase::~SubpanelBase()
{
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
        removeChild(*it);
}

//void SubpanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{
//    if (flags&FLAGS_CONTENT_SIZE_DIRTY)
//        PositionElements();
//}
void SubpanelBase::InitWithContent(SCDFC::PropertiesPanel *_parent, cocos2d::Size s)
{
    parent=_parent;
    setContentSize(s);
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    setBackGroundColor(Color3B::YELLOW);
    setAnchorPoint(Vec2(0,1));
    CreateControls();
}

void PropertiesPanel::OSCInfo::TextFieldEvent(Ref *pSender, TextField::EventType type)
{
    if (NULL==parent->currentControlUnit) return;
    TextField *text=dynamic_cast<TextField*>(pSender);
    switch (type)
    {
        case TextField::EventType::DETACH_WITH_IME:
        {
            if (oscPort==text)
                 parent->currentControlUnit->SetOscPort(std::stoi(text->getStringValue()));
            else if (oscIP==text)
                 parent->currentControlUnit->SetOscIp(text->getStringValue());
        }
        break;
        default:
            break;
    }
}

void PropertiesPanel::OSCInfo::SelectedEvent(Ref* pSender,CheckBox::EventType type)
{
    if (NULL==parent->currentControlUnit) return;
    switch (type)
    {
        case CheckBox::EventType::SELECTED:{
            parent->currentControlUnit->SetOscEnabled(true);
            break;
        }
        case CheckBox::EventType::UNSELECTED:{
             parent->currentControlUnit->SetOscEnabled(false);
            break;
        }
        default:
            break;
    }
}

void PropertiesPanel::OSCInfo::CreateControls()
{
    //Create toggle
    oscToggle=CheckBox::create();
    oscToggle->loadTextures("ToggleOff.png",
                            "ToggleOn.png",
                            "ToggleOn.png",
                            "ToggleOn.png",
                            "ToggleOff.png");
    
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    oscToggle->setAnchorPoint(Vec2(0,1));
    oscToggle->setPosition(Vec2(getContentSize().width/2+getContentSize().width/4-oscToggle->getContentSize().width/2,getContentSize().height));
    oscToggle->addEventListener(CC_CALLBACK_2(PropertiesPanel::OSCInfo::SelectedEvent, this));
    addChild(oscToggle);
    
    //Create toggle label
    toggleLabel = Text::create("Enable OSC","Arial",18);
    toggleLabel->ignoreContentAdaptWithSize(false);
    toggleLabel->setAnchorPoint(Vec2(0,1));
    toggleLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    toggleLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
   // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    toggleLabel->setPosition(Vec2(0,getContentSize().height));
    toggleLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(toggleLabel);
    
    //Create oscPort control
    oscPort=TextField::create();
    //oscIP->setPlaceHolder("No Port");
    oscPort->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    oscPort->setAnchorPoint(Vec2(0,1));
    oscPort->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscPort->setTextAreaSize(oscPort->getContentSize());
    oscPort->setFontSize(20);
    oscPort->setTouchEnabled(true);
    oscPort->setPosition(Vec2(getContentSize().width/2,getContentSize().height-getContentSize().height/4));
    oscPort->setColor(cocos2d::Color3B::BLACK);
    oscPort->addEventListener(CC_CALLBACK_2(PropertiesPanel::OSCInfo::TextFieldEvent, this));
    addChild(oscPort);
    
    //Create oscPort label
    portLabel = Text::create("UDP port:","Arial",18);
    portLabel->ignoreContentAdaptWithSize(false);
    portLabel->setAnchorPoint(Vec2(0,1));
    portLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    portLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    portLabel->setPosition(Vec2(0,getContentSize().height-getContentSize().height/4));
    portLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(portLabel);
    
    //Create oscIP control
    oscIP=TextField::create();
    //oscIP->setPlaceHolder("No Ip");
    oscIP->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    oscIP->setAnchorPoint(Vec2(0,1));
    oscIP->setTextAreaSize(oscIP->getContentSize());
    oscIP->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscIP->setTouchEnabled(true);
    oscIP->setPosition(Vec2(getContentSize().width/2,getContentSize().height/2));
    oscIP->setColor(cocos2d::Color3B::BLACK);
    oscIP->addEventListener(CC_CALLBACK_2(PropertiesPanel::OSCInfo::TextFieldEvent, this));
    addChild(oscIP);
    
    
    //Create oscIP label
    ipLabel = Text::create("IP address:","Arial",18);
    ipLabel->ignoreContentAdaptWithSize(false);
    ipLabel->setAnchorPoint(Vec2(0,1));
    ipLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    ipLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    ipLabel->setPosition(Vec2(0,getContentSize().height/2));
    ipLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(ipLabel);
    
    //Create OSCtag label
    oscTagLabel = Text::create("OSC Tag:","Arial",18);
    oscTagLabel->ignoreContentAdaptWithSize(false);
    oscTagLabel->setAnchorPoint(Vec2(0,1));
    oscTagLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTagLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTagLabel->setPosition(Vec2(0,getContentSize().height/4));
    oscTagLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(oscTagLabel);
    
    //Create OSCtag label
    oscTag = Text::create("","Arial",18);
       // oscTag = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    oscTag->ignoreContentAdaptWithSize(false);
    oscTag->setAnchorPoint(Vec2(0,1));
    oscTag->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTag->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTag->setPosition(Vec2(getContentSize().width/2,getContentSize().height/4));
    oscTag->setColor(cocos2d::Color3B::BLACK);
    addChild(oscTag);
}

void PropertiesPanel::MIDIDevices::CreateControls()
{
    //Create dropDown label
    devicesLabel = Text::create("MIDI devices:","Arial",18);
    devicesLabel->ignoreContentAdaptWithSize(false);
    devicesLabel->setAnchorPoint(Vec2(0,1));
    devicesLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    devicesLabel->setContentSize(Size(getContentSize().width,getContentSize().height/2));
    devicesLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(devicesLabel);
    
    //Create dropDown
//    dropDownCallback.reset(new DropDownMenuCallbackSubPanel(parent));
    devices = DropDownMenu::CreateMenu(Size(getContentSize().width,getContentSize().height/2));
    devices->ignoreContentAdaptWithSize(false);
    devices->setAnchorPoint(Vec2(0,1));
    devices->SetCallback(parent->GetDropDownCallback());
    std::vector<std::string> dropDownData;
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
    PositionElements();
    devices->InitData(dropDownData);
    addChild(devices);
}

void PropertiesPanel::MIDIDevices::UpdateValues()
{
//    std::vector<std::string> dropDownData;
//    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
//        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
//    devices->InitData(dropDownData);
    
    devices->SetSelectedIndex(parent->currentControlUnit->GetMidiOutIndex());
}

void PropertiesPanel::MIDIDevices::PositionElements()
{
    devicesLabel->setPosition(Vec2(0,getContentSize().height));
    devices->SetMenuPosition(Vec2(0,getContentSize().height-devicesLabel->getContentSize().height));
}

void PropertiesPanel::MIDIDevices::CheckForDropDownChanges(DropDownMenu *menu)
{
    int selectedIndex=menu->getCurSelectedIndex();
    if (devices==menu)
        parent->currentControlUnit->SetMidiOutIndex(selectedIndex);
}

void PropertiesPanel::OSCInfo::UpdateValues()
{
    oscToggle->setSelectedState(parent->currentControlUnit->IsOscEnabled());
    std::ostringstream os;
    os<<(parent->currentControlUnit->GetOscPort());
    oscPort->setText(os.str());
    oscIP->setText(parent->currentControlUnit->GetOscIp());
    oscTag->setString(parent->currentControlUnit->GetOscTag());
}

void PropertiesPanel::MIDIInfo::UpdateValues()
{
//    std::vector<std::string> dropDownData;
//    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
//        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
//    devices->InitData(dropDownData);
    midiMessage->SetSelectedIndex(((int)parent->currentControlUnit->GetMidiMessageType())-1);
    controlChange->SetSelectedIndex(parent->currentControlUnit->GetMidiControl());
    channel->SetSelectedIndex(parent->currentControlUnit->GetMidiChannel());
    //velocity->SetSelectedIndex(parent->currentControlUnit->Get());
}

void PropertiesPanel::MIDIInfo::PositionElements()
{
    midiMessageLabel->setPosition(Vec2(0,getContentSize().height));
    midiMessage->SetMenuPosition(Vec2(midiMessageLabel->getContentSize().width,getContentSize().height));
    controlChangeLabel->setPosition(Vec2(0,midiMessage->getPositionY()-midiMessage->getContentSize().height));
    controlChange->SetMenuPosition(Vec2(controlChangeLabel->getContentSize().width,midiMessage->getPositionY()-midiMessage->getContentSize().height));
    channelLabel->setPosition(Vec2(0,controlChangeLabel->getPositionY()-controlChange->getContentSize().height));
    channel->SetMenuPosition(Vec2(channelLabel->getContentSize().width,controlChangeLabel->getPositionY()-controlChange->getContentSize().height));
    velocityLabel->setPosition(Vec2(0,channelLabel->getPositionY()-channel->getContentSize().height));
    velocity->SetMenuPosition(Vec2(velocityLabel->getContentSize().width,channelLabel->getPositionY()-channel->getContentSize().height));
}

void PropertiesPanel::MIDIInfo::CheckForDropDownChanges(DropDownMenu *menu)
{
    int selectedIndex=menu->getCurSelectedIndex();
    if (menu==midiMessage)
        parent->currentControlUnit->SetMidiMessageType((MidiMessageType)(selectedIndex+1));
    else if (menu==controlChange)
        parent->currentControlUnit->SetMidiControl(selectedIndex);
    else if (menu==channel)
        parent->currentControlUnit->SetMidiChannel(selectedIndex);
//    else if (menu==velocity)
//        parent->currentControlUnit->Set(selectedIndex);
    
}

void PropertiesPanel::MIDIInfo::CreateControls()
{
//    DropDownMenu *midiMessage, *controlChange, *channel, *velocity;
//    cocos2d::ui::Text *midiMessageLabel, *controlChangeLabel, *channel, *velocityLabel;

    std::vector<std::string> MidiMessageString;
    MidiMessageString.push_back("NoteOn");
    MidiMessageString.push_back("NoteOff");
    MidiMessageString.push_back("Aftertouch");
    MidiMessageString.push_back("ControlChange");
    MidiMessageString.push_back("ProgramChange");
    MidiMessageString.push_back("PolyKeyPressure");
    MidiMessageString.push_back("PitchBend");
    
    //Create midiMessage label
    midiMessageLabel = Text::create("MIDI Message:","Arial",18);
    addChild(midiMessageLabel);
    midiMessageLabel->ignoreContentAdaptWithSize(false);
    midiMessageLabel->setAnchorPoint(Vec2(0,1));
    midiMessageLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    midiMessageLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    midiMessageLabel->setColor(cocos2d::Color3B::BLACK);

    
    //Create dropDown midiMessage
    midiMessage = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    addChild(midiMessage);
    midiMessage->ignoreContentAdaptWithSize(false);
    midiMessage->setAnchorPoint(Vec2(0,1));
    midiMessage->SetCallback(parent->GetDropDownCallback());
    std::vector<std::string> dropDownData;
    for (int i=0;i<MidiMessageString.size();++i)
        dropDownData.push_back(MidiMessageString[i]);
    midiMessage->InitData(dropDownData);
    
    //Create dropDown label
    controlChangeLabel = Text::create("Control change:","Arial",18);
    addChild(controlChangeLabel);
    controlChangeLabel->ignoreContentAdaptWithSize(false);
    controlChangeLabel->setAnchorPoint(Vec2(0,1));
    controlChangeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    controlChangeLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    controlChangeLabel->setColor(cocos2d::Color3B::BLACK);


    //Create dropDown
    controlChange = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    addChild(controlChange);
    controlChange->ignoreContentAdaptWithSize(false);
    controlChange->setAnchorPoint(Vec2(0,1));
    controlChange->SetCallback(parent->GetDropDownCallback());
    dropDownData.clear();
    for (int i=0;i<120;++i)
    {
        std::ostringstream os;
        os<<"CC: "<<i;
        dropDownData.push_back(os.str());
    }
    controlChange->InitData(dropDownData);
    
    //Create dropDown label
    channelLabel = Text::create("Channel:","Arial",18);
    addChild(channelLabel);
    channelLabel->ignoreContentAdaptWithSize(false);
    channelLabel->setAnchorPoint(Vec2(0,1));
    channelLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    channelLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    channelLabel->setColor(cocos2d::Color3B::BLACK);


    //Create dropDown
    channel = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    addChild(channel);
    channel->ignoreContentAdaptWithSize(false);
    channel->setAnchorPoint(Vec2(0,1));
    channel->SetCallback(parent->GetDropDownCallback());
    dropDownData.clear();
    for (int i=0;i<16;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(os.str());
    }
    channel->InitData(dropDownData);

    //Create dropDown label
    velocityLabel = Text::create("Velocity:","Arial",18);
    addChild(velocityLabel);
    velocityLabel->ignoreContentAdaptWithSize(false);
    velocityLabel->setAnchorPoint(Vec2(0,1));
    velocityLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    velocityLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    velocityLabel->setColor(cocos2d::Color3B::BLACK);


    //Create dropDown
    velocity = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    addChild(velocity);
    velocity->ignoreContentAdaptWithSize(false);
    velocity->setAnchorPoint(Vec2(0,1));
    velocity->SetCallback(parent->GetDropDownCallback());
    dropDownData.clear();
    for (int i=0;i<128;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(os.str());
    }
    velocity->InitData(dropDownData);
    
    PositionElements();


}

#define PROPERTIES_PADDING 20
PropertiesPanel *PropertiesPanel::CreatePropertiesPanel(MainScene *main, cocos2d::Rect r)
{
    PropertiesPanel *panel=(PropertiesPanel*)PropertiesPanel::create();
    panel->InitWithContent(main,r);
    return panel;
}

void PropertiesPanel::PositionElements()
{
    Vector<Node*> _childrens = getChildren();
    float lastPosY=getInnerContainerSize().height;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        lastPosY-=PROPERTIES_PADDING;
        (*it)->setPosition(Vec2(PROPERTIES_PADDING, lastPosY));
        lastPosY-=(*it)->getContentSize().height;
        ((SubpanelBase*)(*it))->PositionElements();
    }
}
void PropertiesPanel::InitLayout()
{
    CalculateInnerHeight();
    PositionElements();
}

void PropertiesPanel::CalculateInnerHeight()
{
    Vector<Node*> _childrens = getChildren();
    int innerHeight=0;
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        innerHeight+=PROPERTIES_PADDING;
        innerHeight+=(*it)->getContentSize().height;
    }
    innerHeight+=PROPERTIES_PADDING;
    setInnerContainerSize(Size(getContentSize().width,innerHeight));
}

void PropertiesPanel::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    dropDownCallback.reset(new DropDownMenuCallbackPanel(this));
    currentControlUnit=NULL;
    
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    setBackGroundColor(Color3B(160,160,160));
    setBounceEnabled(true);
    setInertiaScrollEnabled(true);
    main->addChild(this);
 
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->InitWithContent(this, Size(getContentSize().width-2*PROPERTIES_PADDING,getContentSize().width));
    addChild(sectionOSCInfo);
    
    sectionMIDIDevices=MIDIDevices::create();
    sectionMIDIDevices->InitWithContent(this, Size(getContentSize().width-2*PROPERTIES_PADDING,getContentSize().width/2));
    addChild(sectionMIDIDevices);
    
    sectionMIDIInfo=MIDIInfo::create();
    sectionMIDIInfo->InitWithContent(this, Size(getContentSize().width-2*PROPERTIES_PADDING,getContentSize().width));
    addChild(sectionMIDIInfo);
    
    InitLayout();
}

void PropertiesPanel::Update(SubjectSimple* subject)
{
    currentControlUnit=NULL;
    ItemBase *item=(ItemBase*)subject;
    if (NULL!=item)
        currentControlUnit=item->GetControlUnit();
    sectionOSCInfo->UpdateValues();
    sectionMIDIDevices->UpdateValues();
    sectionMIDIInfo->UpdateValues();
    InitLayout();
}

DropDownMenuCallback *PropertiesPanel::GetDropDownCallback()
{
    return dropDownCallback.get();
}

void PropertiesPanel::OnSelectedDropDownItem(DropDownMenu *menu)
{
    if (NULL==currentControlUnit) return;
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
        ((SubpanelBase*)(*it))->CheckForDropDownChanges(menu);
}
