//
//  Label.cpp
//  ScdfController
//
//  Created by andrea scuderi on 21/03/15.
//
//

#include "Label.h"
#include "Colors.h"

using namespace ScdfCtrl;
using namespace cocos2d;
using namespace cocos2d::ui;

void ControlWithBackground::InitWithContent(cocos2d::Rect r)
{
    setAnchorPoint(Vec2(0,1));
    setContentSize(r.size);
    setPosition(r.origin);
}

ControlWithBackground::~ControlWithBackground()
{
    removeAllChildren();
}

TextWithBackground *TextWithBackground::CreateText(int ctrlID, cocos2d::Rect r, std::string s, std::string fontName, int fontSize)
{
    TextWithBackground *textWB=TextWithBackground::create();
    textWB->InitWithContent(ctrlID,r,s,fontName,fontSize);
    return textWB;
}

#define LABEL_X_OFFSET 2.0

void TextWithBackground::InitWithContent(int ctrlID, cocos2d::Rect r, std::string s, std::string fontName, int fontSize)
{
    ControlWithBackground::InitWithContent(r);
    text = Text::create(s,fontName,fontSize);
    addChild(text,1,ctrlID);
    text->ignoreContentAdaptWithSize(false);
    text->setTextHorizontalAlignment(TextHAlignment::LEFT);
    text->setAnchorPoint(Vec2(0,1));
    text->setTextVerticalAlignment(TextVAlignment::CENTER);
    text->setContentSize(cocos2d::Size(r.size.width-LABEL_X_OFFSET,r.size.height));
    text->setPosition(cocos2d::Vec2(LABEL_X_OFFSET,r.size.height));
    text->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
}

void TextWithBackground::setPosition(const Vec2& position)
{
    Layout::setPosition(position);
    if (text)
        text->setPosition(cocos2d::Vec2(LABEL_X_OFFSET,getContentSize().height));
}

void TextWithBackground::setContentSize(const cocos2d::Size &contentSize)
{
    Layout::setContentSize(contentSize);
    if (text)
        text->setContentSize(cocos2d::Size(contentSize.width-LABEL_X_OFFSET, contentSize.height));
}

void TextWithBackground::SetTextColor(Color3B c)
{
    if (text)
        text->setColor(c);
}

void TextWithBackground::AddTouchCallback(Widget::ccWidgetTouchCallback callback)
{
    text->setTouchEnabled(true);
    text->addTouchEventListener(callback);
}

void TextWithBackground::SetText(std::string s)
{
    text->setString(s);
}

void TextWithBackground::SetAlignement(TextHAlignment hAlign, TextVAlignment vAlign)
{
    text->setTextVerticalAlignment(vAlign);
    text->setTextHorizontalAlignment(hAlign);
}

TextInputWithBackground *TextInputWithBackground::CreateText(int ctrlID, cocos2d::Rect r, std::string s, std::string fontName, int fontSize)
{
    TextInputWithBackground *textWB=TextInputWithBackground::create();
    textWB->InitWithContent(ctrlID,r,s,fontName,fontSize);
    return textWB;
}

void TextInputWithBackground::InitWithContent(int ctrlID, cocos2d::Rect r, std::string s, std::string fontName, int fontSize)
{
    ControlWithBackground::InitWithContent(r);
    text = TextField::create();
    text->setPlaceHolder(s);
    text->setFontName(fontName);
    text->setFontSize(fontSize);
    addChild(text,1,ctrlID);
    text->ignoreContentAdaptWithSize(false);
    text->setTextHorizontalAlignment(TextHAlignment::LEFT);
    text->setAnchorPoint(Vec2(0,1));
    text->setTextVerticalAlignment(TextVAlignment::CENTER);
    text->setContentSize(cocos2d::Size(r.size.width-5,r.size.height));
    text->setPosition(cocos2d::Vec2(5,r.size.height));
    text->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
}

void TextInputWithBackground::AddEventListener(TextField::ccTextFieldCallback callback)
{
    text->addEventListener(callback);
}

void TextInputWithBackground::OnTouchIndirect()
{
    if(!text) return;
    text->setAttachWithIME(true);
    text->update(0);
}

void TextInputWithBackground::SetText(std::string s)
{
    text->setText(s);
}

Toolbar *Toolbar::CreateToolbar(cocos2d::Rect r, float xOffset, float yOffset)
{
    Toolbar *toolbar=Toolbar::create();
    toolbar->setAnchorPoint(Vec2(0,1));
    toolbar->setContentSize(r.size);
    toolbar->setPosition(r.origin);
    toolbar->setClippingEnabled(true);
    toolbar->SetOffsets(xOffset, yOffset);
    return toolbar;
}

void Toolbar::SetOffsets(float _xOffset, float _yOffset)
{
    xOffset=_xOffset;
    yOffset=_yOffset;
}
                        
void Toolbar::AddButton(int ctrlID, cocos2d::Size s, std::vector<std::string> images, Widget::ccWidgetTouchCallback callback)
{
    assert(images.size()>1);
    auto button=Button::create();
    buttons.push_back(new ToolbarButton(button, images[0], images[1]));
    button->loadTextures(images[0], images[1], "");
    button->addTouchEventListener(callback);
    button->setTouchEnabled(true);
    button->setContentSize(s);
    button->setAnchorPoint(Vec2(0,1));
    button->ignoreContentAdaptWithSize(false);
    addChild(button,1,ctrlID);
    UpdateLayout();
}

Toolbar::ToolbarButton *Toolbar::GetButton(int ctrlID)
{
    for (int i=0;i<buttons.size();++i)
    {
        if (buttons[i]->button->getTag()==ctrlID)
            return buttons[i];
    }
    return NULL;
}

void Toolbar::CheckButton(int ctrlID)
{
    ToolbarButton *b=GetButton(ctrlID);
    if (NULL==b) return;
    //b->SetChecked(true);
    b->button->setOpacity(100);

    for (int i=0;i<buttons.size();++i)
    {
        ToolbarButton *bb=buttons[i];
        if (b==bb) continue;
        //bb->SetChecked(false);
        bb->button->setOpacity(255);
    }
}

void Toolbar::RemoveButton(int ctrlID)
{
    ToolbarButton *b=GetButton(ctrlID);
    for (int i=0;i<buttons.size();++i)
    {
        if(b->button!=buttons[i]->button) continue;
        removeChild(buttons[i]->button);
        delete buttons[i];
        buttons.erase(buttons.begin()+i);
        break;
    }
    UpdateLayout();
}

Toolbar::~Toolbar()
{
    for (int i=0;i<buttons.size();++i)
    {
        removeChild(buttons[i]->button);
        delete buttons[i];
    }
}

void Toolbar::UpdateLayout()
{
    float width=getContentSize().width;
    float lastXOffset=xOffset;
    float lastYOffset=getContentSize().height-yOffset;
    float yOffsetMax=0;
    for (int i=0;i<buttons.size();++i)
    {
        if ((buttons[i]->GetSize().width +lastXOffset)>width)
        {
            lastXOffset=xOffset;
            lastYOffset=lastYOffset-yOffsetMax-yOffset;
            yOffsetMax=0;
        }
        buttons[i]->SetPosition(Vec2(lastXOffset,lastYOffset));
        if (buttons[i]->GetSize().height>yOffsetMax)
            yOffsetMax=buttons[i]->GetSize().height;
        lastXOffset=lastXOffset+buttons[i]->GetSize().width+xOffset;
    }
}

void ModalPanel::OnTouch(Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case Widget::TouchEventType::BEGAN:
            if (dynamic_cast<Node*>(pSender)->getTag()!=1)
                Close();
            break;
        default:
            break;
    }
}

void ModalPanel::Close()
{
    auto fadeOut = FadeOut::create(0.2f);
    auto scale = ScaleTo::create(0.15f, 0.0f);
    auto callback = CallFunc::create([this](){
        removeAllChildren();
        getParent()->removeChild(this);
    });
    auto seq = Sequence::create(fadeOut, callback, NULL);
    
    mainPanel->runAction(scale);
    runAction(seq);
}

bool ModalPanel::init()
{
    bool ret=Layout::init();
    setOpacity(0);
    setTouchEnabled(true);
    setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    setBackGroundColor(Color3B::BLACK);
    setBackGroundColorOpacity(100);
    setAnchorPoint(Vec2(0,1));
    setPosition(Vec2(0,Director::getInstance()->getWinSize().height));
    setContentSize(Director::getInstance()->getWinSize());
    addTouchEventListener(CC_CALLBACK_2(ModalPanel::OnTouch, this));

    mainPanel=CreatePanel();
   // addChild(mainPanel,0,1);
    Layout *mainLayout=dynamic_cast<Layout*>(mainPanel);
    if (mainLayout!=NULL)
    {
        mainLayout->setTouchEnabled(true);
        mainLayout->addTouchEventListener(CC_CALLBACK_2(ModalPanel::OnTouch, this));
    }
    mainPanel->setScale(0);
    
    auto fadeIn = FadeIn::create(0.2f);
    auto scale = ScaleTo::create(0.15f, 1.0f);
    runAction(fadeIn);
    mainPanel->runAction(scale);
    return ret;
}

Node *ModalPanel::CreatePanel()
{
    panel=Layout::create();
    panel->setBackGroundImage("popupPanel.png");
    float textureWidth=panel->getBackGroundImageTextureSize().width;
    float textureHeight=panel->getBackGroundImageTextureSize().height;
    
    cocos2d::Rect rr(15, 13, textureWidth-15-23, textureHeight-13-25);
    panel->setBackGroundImageScale9Enabled(true);
    panel->setBackGroundImageCapInsets(rr);
    panel->setAnchorPoint(Vec2(0.5,0.5));
    float sizeBase=250.0;
    panel->setContentSize(cocos2d::Size(1.512*sizeBase, sizeBase));
    panel->setPosition(Vec2(getContentSize().width/2.0,getContentSize().height/2.0));

    text = Text::create("text",Colors::Instance()->GetFontPath(Colors::FontsId::PropHeader),Colors::Instance()->GetFontSize(Colors::FontsId::PopupPanelText));
    panel->addChild(text);
    text->ignoreContentAdaptWithSize(false);
    text->setTextHorizontalAlignment(TextHAlignment::CENTER);
    text->setAnchorPoint(Vec2(0.5,0.5));
    text->setTextVerticalAlignment(TextVAlignment::CENTER);
    text->setContentSize(cocos2d::Size(panel->getContentSize().width-20,panel->getContentSize().height-20));
    text->setPosition(cocos2d::Vec2(panel->getContentSize().width/2.0,panel->getContentSize().height/2.0));
    text->setColor(Colors::Instance()->GetUIColor(Colors::ModalPanelText));
    
    addChild(panel);
    return panel;
}

void ModalPanel::SetText(std::string _text)
{
    text->setString(_text);
}