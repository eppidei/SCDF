//
//  LoadSavePanel.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#include "LoadSavePanel.h"
#include "OsUtilities.h"
#include "Colors.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace ui;

#define ITEM_HEIGHT 35.0
#define CLOSE_ID -10
#define MAINPANEL -11
#define DISCARD_ID -12

Node *LoadSavePanelBase::CreatePanel()
{
    discard=NULL;
    deleteBtn=NULL;
    CreateMain();
    
    CreateControlButton();
    control->addTouchEventListener(CC_CALLBACK_2(LoadSavePanelBase::OnTouchEvent, this));
    control->ignoreContentAdaptWithSize(false);
    control->setAnchorPoint(Vec2(0,1));
//    control->setPosition(Vec2(close->getPositionX()-control->getContentSize().width-10,ITEM_HEIGHT+20));
    control->setPosition(Vec2(GetMainControl()->getPositionX()+GetMainControl()->getContentSize().width+4-control->getContentSize().width,ITEM_HEIGHT+20));
    SetAdditionalButtonsPos();
    addChild(mainPanel,0,1);
    return mainPanel;
}

void SavePanel::CreateMain()
{
    mainPanel=Layout::create();
    mainPanel->setBackGroundImage("SavePopup.png");
    float textureWidth=mainPanel->getBackGroundImageTextureSize().width;
    float textureHeight=mainPanel->getBackGroundImageTextureSize().height;
    
    cocos2d::Rect rr(15, 13, textureWidth-15-23, textureHeight-13-25);
    mainPanel->setBackGroundImageScale9Enabled(true);
    mainPanel->setBackGroundImageCapInsets(rr);
    mainPanel->setAnchorPoint(Vec2(0.5,0.5));
    float sizeBase=250.0;
    mainPanel->setContentSize(cocos2d::Size(1.512*sizeBase, sizeBase));
  //  mainPanel->setPosition(Vec2(getContentSize().width/2.0,getContentSize().height/2.0));
    
    saveFile=TextField::create();
    mainPanel->addChild(saveFile);
    saveFile->ignoreContentAdaptWithSize(false);
    saveFile->setPlaceHolder("Patch name");
    saveFile->setContentSize(cocos2d::Size(0.787*mainPanel->getContentSize().width,0.198*mainPanel->getContentSize().height));
    saveFile->setPosition(Vec2(0.102*mainPanel->getContentSize().width,0.603*mainPanel->getContentSize().height));
    saveFile->setAnchorPoint(Vec2(0,1));
    saveFile->setTextVerticalAlignment(TextVAlignment::CENTER);
    saveFile->setTextHorizontalAlignment(TextHAlignment::LEFT);
    saveFile->setFontName(Colors::Instance()->GetFontPath(Colors::FontsId::LoadSaveElement));
    saveFile->setFontSize(Colors::Instance()->GetFontSize(Colors::FontsId::LoadSaveElement));
    saveFile->setTouchEnabled(true);
    saveFile->setColor(cocos2d::Color3B::BLACK);
    saveFile->addEventListener(CC_CALLBACK_2(SavePanel::TextFieldEventCallback, this));
    
    text = Text::create("",Colors::Instance()->GetFontPath(Colors::FontsId::PropHeader),Colors::Instance()->GetFontSize(Colors::FontsId::LoadSaveElement));
    mainPanel->addChild(text);
    text->ignoreContentAdaptWithSize(false);
    text->setTextHorizontalAlignment(TextHAlignment::CENTER);
    text->setAnchorPoint(Vec2(0,1));
    text->setTextVerticalAlignment(TextVAlignment::CENTER);
    text->setContentSize(cocos2d::Size(saveFile->getContentSize().width,saveFile->getContentSize().height));
    text->setPosition(cocos2d::Vec2(saveFile->getPositionX(),saveFile->getPositionY()-saveFile->getContentSize().height));
    text->setColor(Color3B::RED);
}

void SavePanel::CheckFileExists(std::string _text)
{
    if(scdf::DoesFileExist(_text))
        SetText("This file already exists");
    else
        SetText("");
}

void SavePanel::TextFieldEventCallback(Ref *pSender, TextField::EventType type)
{
    TextField *text=dynamic_cast<TextField*>(pSender);
    switch (type)
    {
        case TextField::EventType::INSERT_TEXT:
        case TextField::EventType::DELETE_BACKWARD:
            CheckFileExists(text->getStringValue());
            break;
        default:
            break;
    }
}

void SavePanel::SetCurrentPatchName(std::string name)
{
    saveFile->setText(name);
    CheckFileExists(name);
}

void LoadPanel::CreateMain()
{
    mainPanel=Layout::create();
    mainPanel->setBackGroundImage("loadPopup.png");
    float textureWidth=mainPanel->getBackGroundImageTextureSize().width;
    float textureHeight=mainPanel->getBackGroundImageTextureSize().height;
    
    cocos2d::Rect rr(14, 13, textureWidth-14-22, textureHeight-13-24);
    mainPanel->setBackGroundImageScale9Enabled(true);
    mainPanel->setBackGroundImageCapInsets(rr);
    mainPanel->setAnchorPoint(Vec2(0.5,0.5));
    float sizeBase=250.0;
    mainPanel->setContentSize(cocos2d::Size(1.214*sizeBase, sizeBase));
  //  mainPanel->setPosition(Vec2(getContentSize().width/2.0,getContentSize().height/2.0));
    
    loadFiles = ListView::create();
    mainPanel->addChild(loadFiles);
    loadFiles->setAnchorPoint(Vec2(0,1));
    loadFiles->setContentSize(cocos2d::Size(0.775*mainPanel->getContentSize().width, 0.377*mainPanel->getContentSize().height));
    loadFiles->setPosition(Vec2(0.09*mainPanel->getContentSize().width, mainPanel->getContentSize().height*0.689/*-(0.313*mainPanel->getContentSize().height)*/));
    InitFilesListView();
    loadFiles->ScrollView::setDirection(ScrollView::Direction::VERTICAL);
    loadFiles->setGravity(ListView::Gravity::CENTER_HORIZONTAL);
    loadFiles->setBackGroundColorType(BackGroundColorType::NONE);
//    loadFiles->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::UIColorsId::WidgetBackGround));
}

void LoadPanel::CreateControlButton()
{
    control = Button::create();
    mainPanel->addChild(control,0,PATCH_LOAD);
    control->loadTextureNormal("btnLoadDefault.png");
    control->loadTexturePressed("btnLoadHover.png");
    control->setContentSize(cocos2d::Size(2.75*ITEM_HEIGHT,ITEM_HEIGHT));
    
    deleteBtn = Button::create();
    mainPanel->addChild(deleteBtn,0,PATCH_DELETE);
    deleteBtn->addTouchEventListener(CC_CALLBACK_2(LoadPanel::OnTouchEvent, this));
    deleteBtn->ignoreContentAdaptWithSize(false);
    deleteBtn->setAnchorPoint(Vec2(0,1));
    deleteBtn->loadTextureNormal("discardDefault.png");
    deleteBtn->loadTexturePressed("discardHover.png");
    deleteBtn->setContentSize(cocos2d::Size(2.75*ITEM_HEIGHT,ITEM_HEIGHT));
}

void LoadPanel::SetAdditionalButtonsPos()
{
    deleteBtn->setPosition(Vec2(GetMainControl()->getPositionX()-4,ITEM_HEIGHT+20));
}

void SavePanel::CreateControlButton()
{
    control = Button::create();
    mainPanel->addChild(control,0,PATCH_SAVE);
    control->loadTextureNormal("btnSaveDefault.png");
    control->loadTexturePressed("btnSaveHover.png");
    control->setContentSize(cocos2d::Size(1.84*ITEM_HEIGHT,ITEM_HEIGHT));
}

void LoadPanel::InitFilesListView()
{
    loadFiles->removeAllItems();
    std::vector<std::string> files;
    scdf::ListFilesInDirectory(scdf::GetPatchesDirectory(), files);
    for (int i=0; i<files.size(); i++)
    {
        cocos2d::Rect r(0,0,loadFiles->getContentSize().width-6,ITEM_HEIGHT);
        TextWithBackground *model=TextWithBackground::CreateText(0,r, files[i], Colors::Instance()->GetFontPath(Colors::FontsId::ItemLabel),Colors::Instance()->GetFontSize(Colors::FontsId::LoadSaveElement));
        model->SetAlignement();
        model->SetTextColor(cocos2d::Color3B::BLACK);
        model->AddTouchCallback(CC_CALLBACK_2(LoadSavePanelBase::OnTouchEvent, this));
        loadFiles->pushBackCustomItem(model);
    }
    loadFiles->refreshView();
    HighLightCurrentItem();
}

void LoadSavePanelBase::OnTouchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    Node *node=dynamic_cast<Node*>(pSender);
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (node->getTag()!=CLOSE_ID)
                OnTouchBegan(node->getTag());
            else
                Close();
            break;
        case Widget::TouchEventType::ENDED:
        case Widget::TouchEventType::CANCELED:
            OnTouchEnded(node->getTag());
            break;
        default:
            break;
    }
}

void LoadSavePanelBase::SetCallback(LoadSavePanelBaseCallback *_callback)
{
    callback=_callback;
    if(callback->GetCurrentPatchName().size())
        SetCurrentPatchName(callback->GetCurrentPatchName());
}

void SavePanel::OnTouchBegan(int nodeTag)
{
    if (nodeTag==PATCH_SAVE)
    {
        callback->OnSavePatch(saveFile->getStringValue(), false);
        Close();
    }
}

void SaveAfterNewPanel::OnTouchBegan(int nodeTag)
{
    if (nodeTag==PATCH_SAVE)
    {
        callback->OnSavePatch(saveFile->getStringValue(), true);
        Close();
    }
    else if (nodeTag==DISCARD_ID)
    {
        callback->OnDiscardPatch();
        Close();
    }
}

void SavePanel::OnTouchEnded(int nodeTag)
{
}

void LoadPanel::OnTouchBegan(int nodeTag)
{
    if (nodeTag==PATCH_LOAD)
    {
        TextWithBackground *t=(TextWithBackground*)(loadFiles->getItem(loadFiles->getCurSelectedIndex()));
        if (t)
            callback->OnLoadPatch(t->GetText());
        Close();
    }
    else if (nodeTag==PATCH_DELETE)
    {
        int index=loadFiles->getCurSelectedIndex();
        TextWithBackground *t=(TextWithBackground*)(loadFiles->getItem(index));
        if (t)
        {
            scdf::DeleteFile(t->GetText());
            loadFiles->removeItem(index);
            if (loadFiles->getItems().size()<=0)
                Close();
        }
    }
    else
        HighLightCurrentItem();
}

void LoadPanel::OnTouchEnded(int nodeTag)
{
    if (nodeTag!=PATCH_LOAD)
        HighLightCurrentItem();
}

void LoadPanel::HighLightCurrentItem()
{
    for (int i=0;i<loadFiles->getItems().size();++i)
    {
        TextWithBackground *model=dynamic_cast<TextWithBackground*>(loadFiles->getItem(i));
        if (NULL==model) continue;
        model->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::NONE);
    }
    TextWithBackground *model=dynamic_cast<TextWithBackground*>(loadFiles->getItem(loadFiles->getCurSelectedIndex()));
    if (NULL==model) return;
    model->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    model->setBackGroundColor(Colors::Instance()->GetUIColor(Colors::WidgetBackGround));
}

void SaveAfterNewPanel::CreateControlButton()
{
    SavePanel::CreateControlButton();
    
    discard = Button::create();
    discard->addTouchEventListener(CC_CALLBACK_2(LoadPanel::OnTouchEvent, this));
    mainPanel->addChild(discard,0,DISCARD_ID);
    discard->loadTextureNormal("discardDefault.png");
    discard->loadTexturePressed("discardHover.png");
    discard->ignoreContentAdaptWithSize(false);
    discard->setAnchorPoint(Vec2(0,1));
    discard->setContentSize(cocos2d::Size(2.75*ITEM_HEIGHT,ITEM_HEIGHT));
}

void SaveAfterNewPanel::SetAdditionalButtonsPos()
{
    discard->setPosition(Vec2(GetMainControl()->getPositionX()-4,ITEM_HEIGHT+20));
}