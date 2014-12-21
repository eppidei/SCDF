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

void PropertiesPanel::OSCInfo::TextFieldEvent(Ref *pSender, TextField::EventType type)
{
    TextField *text=dynamic_cast<TextField*>(pSender);
    switch (type)
    {
        case TextField::EventType::INSERT_TEXT:
            printf("TEXT TEXT\n");
            break;
            
//        case TextField::EventType::DELETE_BACKWARD:
//            _displayValueLabel->setString(CCString::createWithFormat("delete word")->getCString());
//            break;
            
        default:
            break;
    }
}


void PropertiesPanel::OSCInfo::CreateControls()
{
    //Create toggle
    oscToggle=CheckBox::create();
    oscToggle->loadTextures("CloseNormal.png", "CloseSelected.png",
                            "CloseNormal.png",
                            "CloseNormal.png",
                            "CloseNormal.png");
    
    oscToggle->setTouchEnabled(true);
    oscToggle->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
    oscToggle->setAnchorPoint(Vec2(0,1));
    oscToggle->setPosition(Vec2(getContentSize().width/2,getContentSize().height));
    addChild(oscToggle);
    
    //Create toggle label
    toggleLabel = Text::create("Enable OSC","Arial",18);
    toggleLabel->ignoreContentAdaptWithSize(false);
    toggleLabel->setAnchorPoint(Vec2(0,1));
    toggleLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
   // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    toggleLabel->setPosition(Vec2(0,getContentSize().height));
    toggleLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(toggleLabel);
    
    //Create oscPort control
    oscPort=TextField::create();
    oscPort->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
    oscPort->setAnchorPoint(Vec2(0,1));
    oscPort->setTouchEnabled(true);
    oscPort->setPosition(Vec2(getContentSize().width/2,getContentSize().height-getContentSize().height/3));
    oscPort->setColor(cocos2d::Color3B::BLACK);
    oscPort->addEventListener(CC_CALLBACK_2(PropertiesPanel::OSCInfo::TextFieldEvent, this));
    addChild(oscPort);
    
    //Create oscPort label
    portLabel = Text::create("UDP port:","Arial",18);
    portLabel->ignoreContentAdaptWithSize(false);
    portLabel->setAnchorPoint(Vec2(0,1));
    portLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    portLabel->setPosition(Vec2(0,getContentSize().height-getContentSize().height/3));
    portLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(portLabel);
    
    //Create oscIP control
    oscIP=TextField::create();
    oscIP->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
    oscIP->setAnchorPoint(Vec2(0,1));
    oscIP->setTouchEnabled(true);
    oscIP->setPosition(Vec2(getContentSize().width/2,getContentSize().height/3));
    oscIP->setColor(cocos2d::Color3B::BLACK);
    oscIP->addEventListener(CC_CALLBACK_2(PropertiesPanel::OSCInfo::TextFieldEvent, this));
    addChild(oscIP);
    
    
    //Create oscIP label
    ipLabel = Text::create("UDP IP address:","Arial",18);
    ipLabel->ignoreContentAdaptWithSize(false);
    ipLabel->setAnchorPoint(Vec2(0,1));
    ipLabel->setContentSize(Size(getContentSize().width/2,getContentSize().height/3));
    // toggleLabel->setTextHorizontalAlignment(TextHAlignment::CENTER);
    ipLabel->setPosition(Vec2(0,getContentSize().height-2*getContentSize().height/3));
    ipLabel->setColor(cocos2d::Color3B::BLACK);
    addChild(ipLabel);
    
    

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
    setContentSize(r.size);
    setAnchorPoint(Vec2(0,1));
    setPosition(r.origin);
    setBackGroundColorType(Layout::BackGroundColorType::GRADIENT);
    setBackGroundColor(Color3B::BLUE, Color3B::GREEN);
    setBounceEnabled(true);
    setInertiaScrollEnabled(true);
    main->addChild(this);
    
    OSCInfo *oscSection=sectionOSCInfo.create();
    oscSection->setContentSize(Size(getContentSize().width-2*PROPERTIES_PADDING,getContentSize().width-2*PROPERTIES_PADDING));
    oscSection->setPosition(Vec2(PROPERTIES_PADDING,getContentSize().height-PROPERTIES_PADDING));
    oscSection->CreateControls();
  //  oscSection->IniValues(ControlUnit *cUnit)
    addChild(oscSection, 1, OSCInfo::GetID());
//    AddButtonToScrollView<ItemSlider>("CloseNormal.png");
//    AddButtonToScrollView<ItemPad>("CloseSelected.png");
//    AddButtonToScrollView<ItemKnob>("CloseNormal.png");
//    AddButtonToScrollView<ItemKeyboard>("CloseNormal.png");
//    AddButtonToScrollView<ItemSensor1>("CloseNormal.png");
}

void PropertiesPanel::Update(SubjectSimple* theChangedSubject)
{
    
}
