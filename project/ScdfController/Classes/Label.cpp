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

Toolbar *Toolbar::CreateToolbar(cocos2d::Rect r)
{
    Toolbar *toolbar=Toolbar::create();
    toolbar->setAnchorPoint(Vec2(0,1));
    toolbar->setContentSize(r.size);
    toolbar->setPosition(r.origin);
    toolbar->setClippingEnabled(true);
    return toolbar;
}

void Toolbar::AddButton(int ctrlID, cocos2d::Size s, std::vector<std::string> images, Widget::ccWidgetTouchCallback callback)
{
    auto button = CheckBox::create(images[0],images[1], images[1], images[1], images[0]);
  //  if (images.size()>0)
//        button->loadTextureBackGround(images[0]);
//    if (images.size()>1){
////        button->loadTextureBackGroundSelected(images[1]);
//        button->loadTextureFrontCross(images[1]);
   // }
    button->setAnchorPoint(Vec2(0,1));
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(s);
    button->addTouchEventListener(callback);
    button->setSelectedState(false);
    addChild(button,1,ctrlID);
    UpdateLayout();
}
//
void Toolbar::CheckButton(int ctrlID)
{
    CheckBox *b=dynamic_cast<CheckBox*>(getChildByTag(ctrlID));
    if (NULL==b) return;
    b->setSelectedState(true);

    for (auto it=getChildren().begin();it!=getChildren().end();++it)
    {
        CheckBox *bb=dynamic_cast<CheckBox*>(*it);
        if (NULL==bb || b==bb) continue;
        bb->setSelectedState(false);
    }
}
void Toolbar::UpdateLayout()
{
    float width=getContentSize().width;
    float lastXOffset=2.0;
    float lastYOffset=getContentSize().height-2.0;
    float yOffsetMax=0;
    for (auto it=getChildren().begin();it!=getChildren().end();++it)
    {
        if (((*it)->getContentSize().width +lastXOffset)>width)
        {
            lastXOffset=2.0;
            lastYOffset=lastYOffset-yOffsetMax-2.0;
            yOffsetMax=0;
        }
        (*it)->setPosition(Vec2(lastXOffset,lastYOffset));
        if ((*it)->getContentSize().height>yOffsetMax)
            yOffsetMax=(*it)->getContentSize().height;
        lastXOffset=lastXOffset+(*it)->getContentSize().width+2;
    }
}