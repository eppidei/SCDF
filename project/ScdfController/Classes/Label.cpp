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

void TextWithBackground::InitWithContent(int ctrlID, cocos2d::Rect r, std::string s, std::string fontName, int fontSize)
{
    ControlWithBackground::InitWithContent(r);
    text = Text::create(s,fontName,fontSize);
    addChild(text,1,ctrlID);
    text->ignoreContentAdaptWithSize(false);
    text->setTextHorizontalAlignment(TextHAlignment::LEFT);
    text->setAnchorPoint(Vec2(0,1));
    text->setTextVerticalAlignment(TextVAlignment::CENTER);
    text->setContentSize(cocos2d::Size(r.size.width-5,r.size.height));
    text->setPosition(cocos2d::Vec2(5,r.size.height));
    text->setColor(Colors::Instance()->GetUIColor(Colors::LabelText));
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