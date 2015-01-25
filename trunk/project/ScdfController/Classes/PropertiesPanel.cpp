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
#include "ControlUnit.h"
#include "SCDFCItems.h"
//#include "PlatformInfo.h"

using namespace SCDFC;
using namespace cocos2d;
using namespace ui;

void SubpanelBase::DropDownMenuCallbackSubPanel::OnSizeChanged(float oldSize, float newSize)
{
    parent->GetParent()->InitLayout();
}

void SubpanelBase::DropDownMenuCallbackSubPanel::OnSelectItem(DropDownMenu *menu)
{
    parent->OnDropDownSelectionChange(menu);
}

void SubpanelBase::SubPanelItemCallback::OnItemTouchBegan()
{
    parent->GetParent()->EnableScrolling(false);
}
void SubpanelBase::SubPanelItemCallback::OnItemTouchMoved(int value)
{
    ItemBase *item=parent->GetParent()->GetSelectedItem();
    if (NULL==item) return;
    item->SetItemMultiply(value);
}
void SubpanelBase::SubPanelItemCallback::OnItemTouchEnded()
{
    parent->GetParent()->EnableScrolling(true);
}
    
SubpanelBase::~SubpanelBase()
{
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
        removeChild(*it);
}

PropertiesPanel *SubpanelBase::GetParent()
{
    return parent.get();
}

//void SubpanelBase::onSizeChanged()
//{
//    Layout::onSizeChanged();
//    PositionElements();
//    if(parent.get())
//        parent->InitLayout();
//}
void SubpanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    if (flags&FLAGS_CONTENT_SIZE_DIRTY)
    {
        PositionElements();
       // parent->InitLayout();
    }
}

void SubpanelBase::InitWithContent(SCDFC::PropertiesPanel *_parent, cocos2d::Size s)
{
    parent.reset(_parent);
    dropDownCallback.reset(new DropDownMenuCallbackSubPanel(this));
    itemCallback.reset(new SubPanelItemCallback(this));
    setContentSize(s);
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    setBackGroundColor(MAIN_BACK_COLOR);
    setAnchorPoint(Vec2(0,1));
    CreateControls();
    PositionElements();
}

void SubpanelBase::TouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    if (NULL==parent->GetSelectedItem()) return;
    Node* node = dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            switch (node->getTag())
            {
                case PROPERTIES_ITEM_COLOR:
                    //call color picker first and set color from color picker callback?
                    parent->GetSelectedItem()->SetColor(Color3B(255,0,0));
                    UpdateValues();
                    break;
                default:
                    break;
            }
            break;
        case Widget::TouchEventType::MOVED:
            // TODO
            //  break;
        case Widget::TouchEventType::ENDED:
            // TODO
            // break;
        case Widget::TouchEventType::CANCELED:
            // TODO
            // break;
        default:
            // TODO
            break;
    }
}
void SubpanelBase::TextFieldEvent(Ref *pSender, TextField::EventType type)
{
    if (NULL==parent->GetSelectedItem()) return;
    TextField *text=dynamic_cast<TextField*>(pSender);
    switch (type)
    {
        case TextField::EventType::DETACH_WITH_IME:
            switch (text->getTag())
            {
                case PROPERTIES_ITEM_NAME:
                    parent->GetSelectedItem()->SetName(text->getStringValue());
                    break;
                case PROPERTIES_OSC_PORT:
                    parent->GetCurrentControlUnit()->SetOscPort(std::stoi(text->getStringValue()));
                    break;
                case PROPERTIES_OSC_IP:
                    parent->GetCurrentControlUnit()->SetOscIp(text->getStringValue());
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void SubpanelBase::CheckBoxEvent(Ref* pSender,CheckBox::EventType type)
{
    if (NULL==parent->GetCurrentControlUnit()) return;
    CheckBox *checkBox=dynamic_cast<CheckBox*>(pSender);
    bool selected=type==CheckBox::EventType::SELECTED;
    switch (checkBox->getTag())
    {
        case PROPERTIES_OSC_TOGGLE:
            parent->GetCurrentControlUnit()->SetOscEnabled(selected);
            break;
        default:
            break;
    }
}

PropertiesPanel::OSCInfo::OSCInfo()
{
    toggleLabel=portLabel=ipLabel=oscTagLabel=oscTag=NULL;
    oscToggle=NULL;
    oscPort=oscIP=NULL;
}

void PropertiesPanel::OSCInfo::CreateControls()
{
    //Create toggle
    oscToggle=CheckBox::create();
    oscToggle->loadTextures("ToggleOn.png",
                            "ToggleOff.png",
                            "ToggleOff.png",
                            "ToggleOff.png",
                            "ToggleOn.png");

    
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(cocos2d::Size(30,30));
    oscToggle->setAnchorPoint(Vec2(0,1));
    oscToggle->setPosition(Vec2(getContentSize().width/2+getContentSize().width/4-oscToggle->getContentSize().width/2,getContentSize().height));
    oscToggle->addEventListener(CC_CALLBACK_2(SubpanelBase::CheckBoxEvent, this));
    addChild(oscToggle,0,PROPERTIES_OSC_TOGGLE);
    oscToggle->ignoreContentAdaptWithSize(false);
    
    //Create toggle label
    toggleLabel = Text::create("Enable OSC","Arial",18);
    toggleLabel->ignoreContentAdaptWithSize(false);
    toggleLabel->setAnchorPoint(Vec2(0,1));
    toggleLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    toggleLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
   // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    toggleLabel->setPosition(Vec2(0,getContentSize().height));
    toggleLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(toggleLabel);
    
    //Create oscPort control
    oscPort=TextField::create();
    //oscIP->setPlaceHolder("No Port");
    oscPort->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    oscPort->setAnchorPoint(Vec2(0,1));
    oscPort->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscPort->setTextAreaSize(oscPort->getContentSize());
    oscPort->setFontSize(20);
    oscPort->setTouchEnabled(true);
    oscPort->setPosition(Vec2(getContentSize().width/2,getContentSize().height-getContentSize().height/4));
    oscPort->setColor(cocos2d::Color3B::WHITE);
    oscPort->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
    addChild(oscPort, 0, PROPERTIES_OSC_PORT);
    
    //Create oscPort label
    portLabel = Text::create("UDP port:","Arial",18);
    portLabel->ignoreContentAdaptWithSize(false);
    portLabel->setAnchorPoint(Vec2(0,1));
    portLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    portLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    portLabel->setPosition(Vec2(0,getContentSize().height-getContentSize().height/4));
    portLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(portLabel);
    
    //Create oscIP control
    oscIP=TextField::create();
    //oscIP->setPlaceHolder("No Ip");
    oscIP->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    oscIP->setAnchorPoint(Vec2(0,1));
    oscIP->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscIP->setTouchEnabled(true);
    oscIP->setPosition(Vec2(getContentSize().width/2,getContentSize().height/2));
    oscIP->setColor(cocos2d::Color3B::WHITE);
    oscIP->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
    addChild(oscIP, 0 , PROPERTIES_OSC_IP);
    oscIP->setFontName("Arial");
    oscIP->setFontSize(18);
    
    
    //Create oscIP label
    ipLabel = Text::create("IP address:","Arial",18);
    ipLabel->ignoreContentAdaptWithSize(false);
    ipLabel->setAnchorPoint(Vec2(0,1));
    ipLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    ipLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    ipLabel->setPosition(Vec2(0,getContentSize().height/2));
    ipLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(ipLabel);
    
    //Create OSCtag label
    oscTagLabel = Text::create("OSC Tag:","Arial",18);
    oscTagLabel->ignoreContentAdaptWithSize(false);
    oscTagLabel->setAnchorPoint(Vec2(0,1));
    oscTagLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTagLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTagLabel->setPosition(Vec2(0,getContentSize().height/4));
    oscTagLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(oscTagLabel);
    
    //Create OSCtag label
    oscTag = Text::create("","Arial",18);
       // oscTag = DropDownMenu::CreateMenu(Size(getContentSize().width/2,getContentSize().height/4));
    oscTag->ignoreContentAdaptWithSize(false);
    oscTag->setAnchorPoint(Vec2(0,1));
    oscTag->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTag->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTag->setPosition(Vec2(getContentSize().width/2,getContentSize().height/4));
    oscTag->setColor(cocos2d::Color3B::WHITE);
    addChild(oscTag);
}

PropertiesPanel::MIDIDevices::MIDIDevices()
{
    devices=NULL;
    devicesLabel=NULL;
}

void PropertiesPanel::MIDIDevices::CreateControls()
{
    //Create dropDown label
    devicesLabel = Text::create("MIDI devices:","Arial",18);
    addChild(devicesLabel);
    devicesLabel->ignoreContentAdaptWithSize(false);
    devicesLabel->setAnchorPoint(Vec2(0,1));
    devicesLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    devicesLabel->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height/2));
    devicesLabel->setColor(cocos2d::Color3B::WHITE);
    
    //Create dropDown
//    dropDownCallback.reset(new DropDownMenuCallbackSubPanel(parent));
    devices = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,getContentSize().height/2));
    addChild(devices);
    devices->ignoreContentAdaptWithSize(false);
    devices->setAnchorPoint(Vec2(0,1));
    devices->SetCallback(dropDownCallback.get());
    std::vector<std::string> dropDownData;
    dropDownData.push_back("No MIDI connection");
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
    devices->InitData(dropDownData);
}

void PropertiesPanel::MIDIDevices::UpdateValues()
{
//    std::vector<std::string> dropDownData;
//    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
//        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
//    devices->InitData(dropDownData);
    if (NULL==parent->GetCurrentControlUnit()) return;
    devices->SetSelectedIndex(parent->GetCurrentControlUnit()->GetMidiOutIndex()+1);
}

void PropertiesPanel::MIDIDevices::PositionElements()
{
    if (devicesLabel)
        devicesLabel->setPosition(Vec2(0,getContentSize().height));
    if (devices)
        devices->setPosition(Vec2(0,getContentSize().height-devicesLabel->getContentSize().height));
}

void PropertiesPanel::MIDIDevices::OnDropDownSelectionChange(DropDownMenu *menu)
{
    if (NULL==parent->GetCurrentControlUnit()) return;
    int selectedIndex=menu->getCurSelectedIndex();
    if (devices==menu)
        parent->GetCurrentControlUnit()->SetMidiOutIndex(selectedIndex-1);
}

void PropertiesPanel::OSCInfo::UpdateValues()
{
    if (NULL==parent->GetCurrentControlUnit()) return;
    oscToggle->setSelectedState(parent->GetCurrentControlUnit()->IsOscEnabled());
    std::ostringstream os;
    os<<(parent->GetCurrentControlUnit()->GetOscPort());
    oscPort->setText(os.str());
    oscIP->setText(parent->GetCurrentControlUnit()->GetOscIp());
    oscTag->setString(parent->GetCurrentControlUnit()->GetOscTag());
}

void PropertiesPanel::MIDIInfo::UpdateValues()
{
    if (NULL==parent->GetCurrentControlUnit()) return;
//    std::vector<std::string> dropDownData;
//    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
//        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
//    devices->InitData(dropDownData);
    int selectedIndex=-1;
    switch(parent->GetCurrentControlUnit()->GetMidiMessageType())
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
    controlChange->SetSelectedIndex(parent->GetCurrentControlUnit()->GetMidiControl());
    channel->SetSelectedIndex(parent->GetCurrentControlUnit()->GetMidiChannel());
    //velocity->SetSelectedIndex(parent->currentControlUnit->Get());
}

void PropertiesPanel::MIDIInfo::PositionElements()
{
    if (midiMessageLabel)
        midiMessageLabel->setPosition(Vec2(0,getContentSize().height));
    if (midiMessage)
        midiMessage->setPosition(Vec2(midiMessageLabel->getContentSize().width,getContentSize().height));
    if (controlChangeLabel)
        controlChangeLabel->setPosition(Vec2(0,midiMessage->getPositionY()-midiMessage->getContentSize().height));
    if (controlChange)
        controlChange->setPosition(Vec2(controlChangeLabel->getContentSize().width,midiMessage->getPositionY()-midiMessage->getContentSize().height));
    if (channelLabel)
        channelLabel->setPosition(Vec2(0,controlChangeLabel->getPositionY()-controlChange->getContentSize().height));
    if (channel)
        channel->setPosition(Vec2(channelLabel->getContentSize().width,controlChangeLabel->getPositionY()-controlChange->getContentSize().height));
    if (velocityLabel)
        velocityLabel->setPosition(Vec2(0,channelLabel->getPositionY()-channel->getContentSize().height));
    if (velocity)
        velocity->setPosition(Vec2(velocityLabel->getContentSize().width,channelLabel->getPositionY()-channel->getContentSize().height));
}

void PropertiesPanel::MIDIInfo::OnDropDownSelectionChange(DropDownMenu *menu)
{
    if (NULL==parent->GetCurrentControlUnit()) return;
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
        parent->GetCurrentControlUnit()->SetMidiMessageType(msg);
    }
    else if (menu==controlChange)
        parent->GetCurrentControlUnit()->SetMidiControl(selectedIndex);
    else if (menu==channel)
        parent->GetCurrentControlUnit()->SetMidiChannel(selectedIndex);
//    else if (menu==velocity)
//        parent->currentControlUnit->Set(selectedIndex);
    
}

PropertiesPanel::MIDIInfo::MIDIInfo()
{
    midiMessage=controlChange=channel=velocity=NULL;
    midiMessageLabel=controlChangeLabel=channelLabel=velocityLabel=NULL;
}

void PropertiesPanel::MIDIInfo::CreateControls()
{
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
    midiMessageLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    midiMessageLabel->setColor(cocos2d::Color3B::WHITE);

    
    //Create dropDown midiMessage
    midiMessage = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    addChild(midiMessage);
    midiMessage->ignoreContentAdaptWithSize(false);
    midiMessage->setAnchorPoint(Vec2(0,1));
    midiMessage->SetCallback(dropDownCallback.get());
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
    controlChangeLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    controlChangeLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    controlChange = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    addChild(controlChange);
    controlChange->ignoreContentAdaptWithSize(false);
    controlChange->setAnchorPoint(Vec2(0,1));
    controlChange->SetCallback(dropDownCallback.get());
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
    channelLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    channelLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    channel = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    addChild(channel);
    channel->ignoreContentAdaptWithSize(false);
    channel->setAnchorPoint(Vec2(0,1));
    channel->SetCallback(dropDownCallback.get());
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
    velocityLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    velocityLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    velocity = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width/2,getContentSize().height/4));
    addChild(velocity);
    velocity->ignoreContentAdaptWithSize(false);
    velocity->setAnchorPoint(Vec2(0,1));
    velocity->SetCallback(dropDownCallback.get());
    dropDownData.clear();
    for (int i=0;i<128;++i)
    {
        std::ostringstream os;
        os<<i;
        dropDownData.push_back(os.str());
    }
    velocity->InitData(dropDownData);
}

PropertiesPanel::ItemSettings::ItemSettings()
{
    sizeControl=NULL;
    color=NULL;
    name=NULL;
    sizeLabel=colorLabel=nameLabel=NULL;
}

void PropertiesPanel::ItemSettings::PositionElements()
{
    if (colorLabel)
        colorLabel->setPosition(Vec2(0,getContentSize().height));
    if (color)
        color->setPosition(Vec2(getContentSize().width/2,getContentSize().height));
    if (nameLabel)
        nameLabel->setPosition(Vec2(0,colorLabel->getPositionY()-colorLabel->getContentSize().height));
    if (name)
        name->setPosition(Vec2(0,nameLabel->getPositionY()-nameLabel->getContentSize().height));
    if (sizeLabel)
        sizeLabel->setPosition(Vec2(0,name->getPositionY()-name->getContentSize().height));
    if (sizeControl)
        sizeControl->setPosition(Vec2(sizeControl->getContentSize().height/2,sizeLabel->getPositionY()-sizeLabel->getContentSize().height));
}

void PropertiesPanel::ItemSettings::CreateControls()
{
    sizeLabel = Text::create("Item Size:","Arial",18);
    addChild(sizeLabel);
    sizeLabel->ignoreContentAdaptWithSize(false);
    sizeLabel->setAnchorPoint(Vec2(0,1));
    sizeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    sizeLabel->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height/5));
    sizeLabel->setColor(cocos2d::Color3B::WHITE);
    
    colorLabel = Text::create("Item Color:","Arial",18);
    addChild(colorLabel);
    colorLabel->ignoreContentAdaptWithSize(false);
    colorLabel->setAnchorPoint(Vec2(0,1));
    colorLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    colorLabel->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/5));
    colorLabel->setColor(cocos2d::Color3B::WHITE);
    
    nameLabel = Text::create("Item Name:","Arial",18);
    addChild(nameLabel);
    nameLabel->ignoreContentAdaptWithSize(false);
    nameLabel->setAnchorPoint(Vec2(0,1));
    nameLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    nameLabel->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height/5));
    nameLabel->setColor(cocos2d::Color3B::WHITE);
    
    name=TextField::create();
    addChild(name, 0, PROPERTIES_ITEM_NAME);
    name->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height/5));
    name->setAnchorPoint(Vec2(0,1));
    name->setTextVerticalAlignment(TextVAlignment::CENTER);
    name->setFontSize(20);
    name->setTouchEnabled(true);
    name->setColor(cocos2d::Color3B::WHITE);
    name->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
    
    color = Layout::create();
    addChild(color,0,PROPERTIES_ITEM_COLOR);
    color->setTouchEnabled(true);
    color->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    color->ignoreContentAdaptWithSize(false);
    color->setAnchorPoint(Vec2(0,1));
    color->setContentSize(cocos2d::Size(getContentSize().width/2,getContentSize().height/5));
    color->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEvent, this));
    
    sizeControl = dynamic_cast<ItemSlider*>(ItemBase::CreateItem(cocos2d::Rect(0,0,getContentSize().width-getContentSize().height/5, getContentSize().height/5), ITEM_SLIDER_ID));
    addChild(sizeControl,9,3);
    sizeControl->SetVertical(false);
    sizeControl->SetRange(1, 5);
    sizeControl->SetValue(1);
    sizeControl->SetCallback(itemCallback.get());
    sizeControl->SetColor(Color3B::WHITE);
}

void PropertiesPanel::ItemSettings::UpdateValues()
{
    if (NULL==parent->GetSelectedItem()) return;
    sizeControl->SetValue(parent->GetSelectedItem()->GetSizeMultiply());
    color->setBackGroundColor(parent->GetSelectedItem()->GetColor());
    name->setText(parent->GetSelectedItem()->GetName());
}

#define SUBPANEL_DISTANCE 20
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
        lastPosY-=SUBPANEL_DISTANCE;
        (*it)->setPosition(Vec2(SUBPANEL_DISTANCE, lastPosY));
        lastPosY-=(*it)->getContentSize().height;
       // ((SubpanelBase*)(*it))->PositionElements();
    }
}

//void PropertiesPanel::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
//{
//    if (flags&FLAGS_CONTENT_SIZE_DIRTY)
//        PositionElements();
//}

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
        innerHeight+=SUBPANEL_DISTANCE;
        innerHeight+=(*it)->getContentSize().height;
    }
    innerHeight+=SUBPANEL_DISTANCE;
    setInnerContainerSize(cocos2d::Size(getContentSize().width,innerHeight));
}

void PropertiesPanel::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    selectedItem=NULL;
    
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    setBackGroundColor(MAIN_BACK_COLOR);
    setBounceEnabled(true);
    setInertiaScrollEnabled(true);
    main->addChild(this);
 
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().width));
    addChild(sectionOSCInfo);
    
    sectionMIDIDevices=MIDIDevices::create();
    sectionMIDIDevices->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().width/2));
    addChild(sectionMIDIDevices);
    
    sectionMIDIInfo=MIDIInfo::create();
    sectionMIDIInfo->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().width));
    addChild(sectionMIDIInfo);
    
    sectionItemSettings=ItemSettings::create();
    sectionItemSettings->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().width));
    addChild(sectionItemSettings);
    
    InitLayout();
}

void PropertiesPanel::Update(SubjectSimple* subject)
{
    selectedItem=(ItemBase*)subject;
    sectionOSCInfo->UpdateValues();
    sectionMIDIDevices->UpdateValues();
    sectionMIDIInfo->UpdateValues();
    sectionItemSettings->UpdateValues();
}

ItemBase *PropertiesPanel::GetSelectedItem()
{
    return selectedItem;
}

ScdfCtrl::ControlUnit *PropertiesPanel::GetCurrentControlUnit()
{
    if(NULL==selectedItem) return NULL;
    return selectedItem->GetControlUnit();
}

void PropertiesPanel::EnableScrolling(bool enable)
{
    setDirection(enable?Direction::VERTICAL:Direction::NONE);
}