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

#define PROPERTIES_PADDING 20
bool PropertiesPanel::OSCInfo::init()
{
    bool ret=Layout::init();
    setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    setBackGroundColor(Color3B::YELLOW);
    setAnchorPoint(Vec2(0,1));
    return ret;
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

void PropertiesPanel::OSCInfo::TextFieldEvent(Ref *pSender, TextField::EventType type)
{
    if (NULL==((PropertiesPanel*)getParent())->currentControlUnit) return;
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
    if (NULL==((PropertiesPanel*)getParent())->currentControlUnit) return;
    switch (type)
    {
        case CheckBox::EventType::SELECTED:
            parent->currentControlUnit->SetOscEnabled(true);
            break;
            
        case CheckBox::EventType::UNSELECTED:
             parent->currentControlUnit->SetOscEnabled(false);
            break;
        default:
            break;
    }
}


void PropertiesPanel::OSCInfo::CreateControls(PropertiesPanel *_parent)
{
    parent=_parent;
    //Create toggle
    oscToggle=CheckBox::create();
    oscToggle->loadTextures("CloseNormal.png", "CloseSelected.png",
                            "CloseNormal.png",
                            "CloseNormal.png",
                            "CloseNormal.png");
    
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    oscToggle->setAnchorPoint(Vec2(0,1));
    oscToggle->setPosition(Vec2(getContentSize().width/2+getContentSize().width/4-oscToggle->getContentSize().width/2,getContentSize().height));
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
    oscTag->ignoreContentAdaptWithSize(false);
    oscTag->setAnchorPoint(Vec2(0,1));
    oscTag->setTextVerticalAlignment(TextVAlignment::CENTER);
    oscTag->setContentSize(Size(getContentSize().width/2,getContentSize().height/4));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    oscTag->setPosition(Vec2(getContentSize().width/2,getContentSize().height/4));
    oscTag->setColor(cocos2d::Color3B::BLACK);
    addChild(oscTag);
    
    

}
PropertiesPanel *PropertiesPanel::CreatePropertiesPanel(MainScene *main, cocos2d::Rect r)
{
    PropertiesPanel *panel=(PropertiesPanel*)PropertiesPanel::create();
    panel->InitWithContent(main,r);
    return panel;
}

void PropertiesPanel::InitWithContent(MainScene *main,cocos2d::Rect r)
{
   // parent=main;
    currentControlUnit=NULL;
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::GRADIENT);
    setBackGroundColor(Color3B::BLUE, Color3B::GREEN);
    setBounceEnabled(true);
    setInertiaScrollEnabled(true);
    main->addChild(this);
    
    sectionOSCInfo=OSCInfo::create();
    sectionOSCInfo->setContentSize(Size(getContentSize().width-2*PROPERTIES_PADDING,getContentSize().width-2*PROPERTIES_PADDING));
    sectionOSCInfo->setPosition(Vec2(PROPERTIES_PADDING,getContentSize().height-PROPERTIES_PADDING));
    sectionOSCInfo->CreateControls(this);
    addChild(sectionOSCInfo, 1, OSCInfo::GetID());
    
}

void PropertiesPanel::Update(SubjectSimple* subject)
{
    currentControlUnit=NULL;
    ItemBase *item=(ItemBase*)subject;
    if (NULL!=item)
        currentControlUnit=item->GetControlUnit();
    sectionOSCInfo->UpdateValues();
}
