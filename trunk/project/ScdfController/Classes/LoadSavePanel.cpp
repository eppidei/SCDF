//
//  LoadSavePanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#include "LoadSavePanel.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

void LoadSavePanel::InitPanel()
{
    sectionSave=SavePatch::create();
    sectionSave->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().width));
    addChild(sectionSave);
    
    sectionLoad=LoadPatch::create();
    sectionLoad->InitWithContent(this, cocos2d::Size(getContentSize().width-2*SUBPANEL_DISTANCE,getContentSize().height-3*SUBPANEL_DISTANCE-sectionSave->getContentSize().height));
    addChild(sectionLoad);
}

void LoadSavePanel::UpdateSubpanels()
{
    sectionLoad->Update();
    sectionSave->Update();
}

SavePatch::SavePatch()
{
    saveButton=NULL;
    saveFile=NULL;
}

LoadPatch::LoadPatch()
{
    loadButton=NULL;
    loadFiles=NULL;
}

void LoadPatch::Update()
{
    InitFilesListView();
}

void SavePatch::PositionElements()
{
    if (saveButton)
        saveButton->setPosition(Vec2(0,getContentSize().height));
    if (saveFile)
        saveFile->setPosition(Vec2(0,saveButton->getPositionY()-saveButton->getContentSize().height));
}

void LoadPatch::PositionElements()
{
    if (loadButton)
        loadButton->setPosition(Vec2(0,getContentSize().height));
    if (loadFiles)
        loadFiles->setPosition(Vec2(0,loadButton->getPositionY()-loadButton->getContentSize().height));
}

void SavePatch::CreateControls()
{
    saveButton = Text::create("SAVE","Arial",16);
    saveButton->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    addChild(saveButton);
    saveButton->ignoreContentAdaptWithSize(false);
    saveButton->setAnchorPoint(Vec2(0,1));
    saveButton->setTextHorizontalAlignment(TextHAlignment::CENTER);
    saveButton->setTextVerticalAlignment(TextVAlignment::CENTER);
    saveButton->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    saveButton->setColor(cocos2d::Color3B::WHITE);
    
    saveFile=TextField::create();
    saveFile->ignoreContentAdaptWithSize(false);
    saveFile->setPlaceHolder("Insert name");
    saveFile->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    saveFile->setAnchorPoint(Vec2(0,1));
    saveFile->setTextVerticalAlignment(TextVAlignment::CENTER);
    saveFile->setTextHorizontalAlignment(TextHAlignment::CENTER);
    saveFile->setFontSize(20);
    saveFile->setTouchEnabled(true);
    saveFile->setColor(cocos2d::Color3B::WHITE);
    saveFile->addEventListener(CC_CALLBACK_2(SubpanelBase::TextFieldEventCallback, this));
    addChild(saveFile);
}

void LoadPatch::CreateControls()
{
    loadButton = Text::create("LOAD","Arial",16);
    loadButton->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    addChild(loadButton,0,PATCH_LOAD);
    loadButton->ignoreContentAdaptWithSize(false);
    loadButton->setAnchorPoint(Vec2(0,1));
    loadButton->setTextHorizontalAlignment(TextHAlignment::CENTER);
    loadButton->setTextVerticalAlignment(TextVAlignment::CENTER);
    loadButton->setContentSize(cocos2d::Size(getContentSize().width,SUBPANEL_ITEM_HEIGHT));
    loadButton->setColor(cocos2d::Color3B::WHITE);
    
    loadFiles = ListView::create();
    InitFilesListView();
    loadFiles->setContentSize(cocos2d::Size(getContentSize().width,getContentSize().height-SUBPANEL_ITEM_HEIGHT));
    addChild(loadFiles);
    loadFiles->ScrollView::setDirection(ScrollView::Direction::VERTICAL);
    loadFiles->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    loadFiles->setAnchorPoint(Vec2(0,1));
    loadFiles->setBackGroundColorType(BackGroundColorType::NONE);
}

void LoadPatch::InitFilesListView()
{
    loadFiles->removeAllItems();
    const int numFiles=20;
    for (int i=0; i<numFiles; i++)
    {
        std::ostringstream os;
        os<<"File "<<i;
        Text *model = Text::create(os.str(),"Arial",20);
        model->setTouchEnabled(true);
        model->setContentSize(cocos2d::Size(getContentSize().width-6,SUBPANEL_ITEM_HEIGHT));
        model->ignoreContentAdaptWithSize(false);
        model->setTextVerticalAlignment(TextVAlignment::CENTER);
        model->setTextHorizontalAlignment(TextHAlignment::CENTER);
        model->setColor(cocos2d::Color3B::WHITE);
        model->setBright(true);
        model->setTouchScaleChangeEnabled(true);
        loadFiles->pushBackCustomItem(model);
        model->addTouchEventListener(CC_CALLBACK_2(SubpanelBase::TouchEventCallback, this));
    }
    loadFiles->refreshView();
}

void LoadPatch::HighLightCurrentItem()
{
    for (int i=0;i<loadFiles->getItems().size();++i)
        loadFiles->getItem(i)->setHighlighted(false);
    loadFiles->getItem(loadFiles->getCurSelectedIndex())->setHighlighted(true);
}

void SavePatch::OnTouchEventBegan(Node *widget)
{
    
}

void LoadPatch::OnTouchEventBegan(Node *widget)
{
    HighLightCurrentItem();
}

void LoadPatch::OnTouchEventEnded(Node *widget)
{
    if (widget->getTag()==PATCH_LOAD)
        int pippo=0;
    else
    HighLightCurrentItem();
}