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

void TextWithBackground::SetTextColor(Color3B &c)
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
    b->SetChecked(true);

    for (int i=0;i<buttons.size();++i)
    {
        ToolbarButton *bb=buttons[i];
        if (b==bb) continue;
        bb->SetChecked(false);
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