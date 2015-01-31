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

#define VISIBILITY_CHECK \
        if (NULL==parent->GetSelectedItem()) \
        {   \
            if (isVisible()) \
                setVisible(false); \
            return; \
        } \
        if (!isVisible()) \
            setVisible(true); \

#define SUBPANEL_ITEM_HEIGHT 45.0

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

void SubpanelBase::CalculateHeight()
{
    int height=0;
    Vector<Node*> _childrens = getChildren();
    for (auto it=_childrens.begin(); it!=_childrens.end(); ++it)
    {
        if (!(*it)->isVisible() || (*it)->getPositionX()>getContentSize().width/2.0) continue;
        height+=SUBPANEL_ITEM_HEIGHT;//(*it)->getContentSize().height;
    }
    Resize(height);
}

void SubpanelBase::HideElement(Node *n, bool hide)
{
    n->setVisible(!hide);
   // CalculateHeight();
}

void SubpanelBase::InitChildrensVisibilityAndPos()
{
    CalculateHeight();
    PositionElements();
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

void SubpanelBase::Resize(float newHeight)
{
    auto modifyHeight = ActionTween::create(0.1, "height", getContentSize().height, newHeight);
    runAction(modifyHeight);
}

void SubpanelBase::updateTweenAction(float value, const std::string& key)
{
    if ("height"==key){
        setContentSize(cocos2d::Size(getContentSize().width, value));
        parent->InitLayout();
    }
    
}

void SubpanelBase::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(0, getContentSize().height), Vec2(getContentSize().width, 0), Color4F(((float)MAIN_BACK_COLOR_LIGHT.r)/255.f,((float)MAIN_BACK_COLOR_LIGHT.g)/255.f,((float)MAIN_BACK_COLOR_LIGHT.b)/255.f,1.0));
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
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
//    setBackGroundColor(MAIN_BACK_COLOR);
    setAnchorPoint(Vec2(0,1));
    CreateControls();
    PositionElements();
    InitChildrensVisibilityAndPos();
}
#include "extensions/GUI/CCControlExtension/CCControlHuePicker.h"
using namespace cocos2d::extension;
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
                {
                    //call color picker first and set color from color picker callback?
//                    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("huePickerBackground.png");
                 //   addChild(ControlHuePicker::create(this, Vec2(getPositionX()+getContentSize().width,500)));
                    parent->GetSelectedItem()->SetColor(Color3B(255,0,0));
                    UpdateValues();
                }
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
    InitChildrensVisibilityAndPos();
}

void PropertiesPanel::OSCInfo::InitChildrensVisibilityAndPos()
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
    oscToggle->setSelectedState(false);
    oscToggle->addEventListener(CC_CALLBACK_2(SubpanelBase::CheckBoxEvent, this));
    addChild(oscToggle,0,PROPERTIES_OSC_TOGGLE);
    oscToggle->ignoreContentAdaptWithSize(false);
    
    //Create toggle label
    toggleLabel = Text::create("ENABLE OSC","Arial",16);
    toggleLabel->ignoreContentAdaptWithSize(false);
    toggleLabel->setAnchorPoint(Vec2(0,1));
    toggleLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    toggleLabel->setContentSize(cocos2d::Size(getContentSize().width/2,SUBPANEL_ITEM_HEIGHT));
    toggleLabel->setColor(cocos2d::Color3B::WHITE);
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
    oscPort->setColor(cocos2d::Color3B::WHITE);
    oscPort->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
    addChild(oscPort, 0, PROPERTIES_OSC_PORT);
    
    //Create oscPort label
    portLabel = Text::create("UDP PORT","Arial",16);
    portLabel->ignoreContentAdaptWithSize(false);
    portLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    portLabel->setAnchorPoint(Vec2(0,1));
    portLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    portLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    portLabel->setColor(cocos2d::Color3B::WHITE);
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
    oscIP->setColor(cocos2d::Color3B::WHITE);
    oscIP->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
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
    ipLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(ipLabel);
    
    //Create OSCtag label
    oscTagLabel = Text::create("OSC TAG","Arial",16);
    oscTagLabel->ignoreContentAdaptWithSize(false);
    oscTagLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTagLabel->setAnchorPoint(Vec2(0,1));
    oscTagLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTagLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscTagLabel->setColor(cocos2d::Color3B::WHITE);
    addChild(oscTagLabel);
    
    //Create OSCtag label
    oscTag = Text::create("","Arial",16);
    oscTag->setString("No Tag");
    oscTag->ignoreContentAdaptWithSize(false);
    oscTag->setAnchorPoint(Vec2(0,1));
    oscTag->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTag->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTag->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    oscTag->setColor(cocos2d::Color3B::WHITE);
    addChild(oscTag);
    PositionElements();
    InitChildrensVisibilityAndPos();
}

void PropertiesPanel::OSCInfo::PositionElements()
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

PropertiesPanel::MIDIDevices::MIDIDevices()
{
    devices=NULL;
    devicesLabel=NULL;
}

void PropertiesPanel::MIDIDevices::CreateControls()
{
    //Create dropDown label
    devicesLabel = Text::create("MIDI DEVICES","Arial",16);
    addChild(devicesLabel);
    devicesLabel->ignoreContentAdaptWithSize(false);
    devicesLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    devicesLabel->setAnchorPoint(Vec2(0,1));
    devicesLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    devicesLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    devicesLabel->setColor(cocos2d::Color3B::WHITE);
    
    devices = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(devices);
    devices->ignoreContentAdaptWithSize(false);
    devices->setAnchorPoint(Vec2(0,1));
    devices->SetCallback(dropDownCallback.get());
    std::vector<std::string> dropDownData;
    dropDownData.push_back("No MIDI connection");
    for (int i=0;i<Scdf::MidiOutConnection::GetNumAvailableOutputs();++i)
        dropDownData.push_back(Scdf::MidiOutConnection::GetOutputName(i));
    devices->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
}

void PropertiesPanel::MIDIDevices::UpdateValues()
{
    if (parent->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
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
    if (parent->GetSelectedItem()==NULL) return;//VISIBILITY_CHECK
    
    oscToggle->setSelectedState(parent->GetCurrentControlUnit()->IsOscEnabled());
    std::ostringstream os;
    os<<(parent->GetCurrentControlUnit()->GetOscPort());
    oscPort->setText(os.str());
    oscIP->setText(parent->GetCurrentControlUnit()->GetOscIp());
    oscTag->setString(parent->GetCurrentControlUnit()->GetOscTag());
    InitChildrensVisibilityAndPos();
}

void PropertiesPanel::MIDIInfo::UpdateValues()
{
    VISIBILITY_CHECK
    
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
    velocity->SetSelectedIndex(parent->GetSelectedItem()->GetValue());
    velocity->setEnabled(dynamic_cast<ItemSlider*>(parent->GetSelectedItem())==NULL);
}

void PropertiesPanel::MIDIInfo::PositionElements()
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

void PropertiesPanel::MIDIInfo::InitControlMenuValue()
{
    std::vector<std::string> dropDownData;
    if (midiMessage->getCurSelectedIndex()<2)
    {
        static std::string note[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
        int octave=-1;
        for (int i=0;i<127;++i)
        {
            std::ostringstream os;
            os<<note[i%12]<<octave;
            dropDownData.push_back(os.str());
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
            dropDownData.push_back(os.str());
        }
        controlChangeLabel->setString("CONTROL CHANGE");
    }
    controlChange->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
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
        InitControlMenuValue();
    }
    else if (menu==controlChange)
        parent->GetCurrentControlUnit()->SetMidiControl(selectedIndex);
    else if (menu==channel)
        parent->GetCurrentControlUnit()->SetMidiChannel(selectedIndex);
    else if (menu==velocity)
        parent->GetSelectedItem()->SetValue(selectedIndex);
    
}

PropertiesPanel::MIDIInfo::MIDIInfo()
{
    midiMessage=controlChange=channel=velocity=NULL;
    midiMessageLabel=controlChangeLabel=channelLabel=velocityLabel=NULL;
}

void PropertiesPanel::MIDIInfo::CreateControls()
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
    midiMessageLabel->setColor(cocos2d::Color3B::WHITE);

    
    //Create dropDown midiMessage
    midiMessage = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    addChild(midiMessage);
    midiMessage->ignoreContentAdaptWithSize(false);
    midiMessage->setAnchorPoint(Vec2(0,1));
    midiMessage->SetCallback(dropDownCallback.get());
    std::vector<std::string> dropDownData;
    for (int i=0;i<MidiMessageString.size();++i)
        dropDownData.push_back(MidiMessageString[i]);
    midiMessage->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
    
    //Create dropDown label
    controlChangeLabel = Text::create("CONTROL CHANGE","Arial",16);
    addChild(controlChangeLabel);
    controlChangeLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    controlChangeLabel->ignoreContentAdaptWithSize(false);
    controlChangeLabel->setAnchorPoint(Vec2(0,1));
    controlChangeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    controlChangeLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    controlChangeLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    controlChange = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
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
    channelLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    channel = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
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
    channel->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);

    //Create dropDown label
    velocityLabel = Text::create("VELOCITY","Arial",16);
    addChild(velocityLabel);
    velocityLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    velocityLabel->ignoreContentAdaptWithSize(false);
    velocityLabel->setAnchorPoint(Vec2(0,1));
    velocityLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    velocityLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    velocityLabel->setColor(cocos2d::Color3B::WHITE);


    //Create dropDown
    velocity = DropDownMenu::CreateMenu(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
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
    velocity->InitData(dropDownData, SUBPANEL_ITEM_HEIGHT);
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
    if (nameLabel)
        nameLabel->setPosition(Vec2(0,getContentSize().height));
    if (name)
        name->setPosition(Vec2(0,nameLabel->getPositionY()-nameLabel->getContentSize().height));
    if (colorLabel)
        colorLabel->setPosition(Vec2(0,name->getPositionY()-name->getContentSize().height));
    if (color)
        color->setPosition(Vec2(getContentSize().width/2-color->getContentSize().width/2,colorLabel->getPositionY()-colorLabel->getContentSize().height));

    if (sizeLabel)
        sizeLabel->setPosition(Vec2(0,color->getPositionY()-color->getContentSize().height));
    if (sizeControl)
        sizeControl->setPosition(Vec2(0,sizeLabel->getPositionY()-sizeLabel->getContentSize().height));
}

void PropertiesPanel::ItemSettings::CreateControls()
{
    sizeLabel = Text::create("ITEM SIZE","Arial",16);
    addChild(sizeLabel);
    sizeLabel->ignoreContentAdaptWithSize(false);
    sizeLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    sizeLabel->setAnchorPoint(Vec2(0,1));
    sizeLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    sizeLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    sizeLabel->setColor(cocos2d::Color3B::WHITE);
    
    colorLabel = Text::create("ITEM COLOR","Arial",16);
    addChild(colorLabel);
    colorLabel->ignoreContentAdaptWithSize(false);
    colorLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    colorLabel->setAnchorPoint(Vec2(0,1));
    colorLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    colorLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    colorLabel->setColor(cocos2d::Color3B::WHITE);
    
    nameLabel = Text::create("ITEM NAME","Arial",16);
    addChild(nameLabel);
    nameLabel->ignoreContentAdaptWithSize(false);
    nameLabel->setAnchorPoint(Vec2(0,1));
    nameLabel->setTextVerticalAlignment(TextVAlignment::CENTER);
    nameLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    nameLabel->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    nameLabel->setColor(cocos2d::Color3B::WHITE);
    
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
    name->setColor(cocos2d::Color3B::WHITE);
    name->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEvent, this));
    
    color = Layout::create();
    addChild(color,0,PROPERTIES_ITEM_COLOR);
    color->setTouchEnabled(true);
    color->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    color->ignoreContentAdaptWithSize(false);
    color->setAnchorPoint(Vec2(0,1));
    color->setContentSize(cocos2d::Size(getContentSize().width/3,SUBPANEL_ITEM_HEIGHT));
    color->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEvent, this));
    
    sizeControl = dynamic_cast<ItemSlider*>(ItemBase::CreateItem(cocos2d::Rect(0,0,getContentSize().width,SUBPANEL_ITEM_HEIGHT), ITEM_SLIDER_ID));
    addChild(sizeControl,0,3);
    sizeControl->SetVertical(false);
    sizeControl->setAnchorPoint(Vec2(0,1));
    sizeControl->SetRange(1, 5);
    sizeControl->SetValue(1);
    sizeControl->SetCallback(itemCallback.get());
    sizeControl->SetColor(MAIN_BACK_COLOR_LIGHT);
}

void PropertiesPanel::ItemSettings::UpdateValues()
{
    VISIBILITY_CHECK
    
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
        if (!(*it)->isVisible()) continue;
        lastPosY-=SUBPANEL_DISTANCE;
        (*it)->setPosition(Vec2(SUBPANEL_DISTANCE, lastPosY));
        lastPosY-=(*it)->getContentSize().height;
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
        if (!(*it)->isVisible()) continue;
        innerHeight+=SUBPANEL_DISTANCE;
        innerHeight+=(*it)->getContentSize().height;
    }
    innerHeight+=SUBPANEL_DISTANCE;
    setInnerContainerSize(cocos2d::Size(getContentSize().width,innerHeight));
}

void PropertiesPanel::InitWithContent(MainScene *main,cocos2d::Rect r)
{
    selectedItem=NULL;
    
    //setClippingEnabled(false);
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::NONE);
  //  setBackGroundColor(MAIN_BACK_COLOR);
   // setBounceEnabled(true);
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
    
    UpdateAll();
}

void PropertiesPanel::UpdateAll()
{
    sectionOSCInfo->UpdateValues();
    sectionMIDIDevices->UpdateValues();
    sectionMIDIInfo->UpdateValues();
    sectionItemSettings->UpdateValues();
    InitLayout();
}

void PropertiesPanel::Update(SubjectSimple* subject, SCDFC_EVENTS event)
{
    if (NULL==subject)
        selectedItem=GetSelectedItem();
    else if (NULL!=dynamic_cast<ItemKeyboard*>(subject))
        selectedItem=dynamic_cast<ItemKeyboard*>(subject)->GetSelectedPad();
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
            sectionMIDIInfo->UpdateValues();
            break;
        case SCDFC_EVENTS_Remove_Item:
            selectedItem=NULL;
        default:
            UpdateAll();
            break;
    }
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

void PropertiesPanel::draw(Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    DrawPrimitives::drawSolidRect(Vec2(0, getInnerContainerSize().height), Vec2(getInnerContainerSize().width, 0), Color4F(((float)MAIN_BACK_COLOR.r)/255.f,((float)MAIN_BACK_COLOR.g)/255.f,((float)MAIN_BACK_COLOR.b)/255.f,1.0));
}

void PropertiesPanel::EnableScrolling(bool enable)
{
    setDirection(enable?Direction::VERTICAL:Direction::NONE);
}
