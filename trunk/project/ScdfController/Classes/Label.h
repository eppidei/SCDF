//
//  Label.h
//  ScdfController
//
//  Created by andrea scuderi on 21/03/15.
//
//

#ifndef __ScdfController__Label__
#define __ScdfController__Label__

#include <stdio.h>
#include "SCDFCDefinitions.h"

namespace ScdfCtrl
{
    class ControlWithBackground : public cocos2d::ui::Layout
    {
    protected:
        void InitWithContent(cocos2d::Rect r);
    public:
        ~ControlWithBackground();
    };
    
    class TextWithBackground : public ControlWithBackground
    {
        cocos2d::ui::Text *text;
        void InitWithContent(int ctrlID, cocos2d::Rect r, std::string text, std::string fontName, int fontSize);
    public:
        static TextWithBackground *CreateText(int ctrlID, cocos2d::Rect r, std::string text, std::string fontName, int fontSize);
        void AddTouchCallback(Widget::ccWidgetTouchCallback callback);
        void SetText(std::string s);
        void SetAlignement(cocos2d::TextHAlignment hAlign=cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment vAlign=cocos2d::TextVAlignment::CENTER);
        CREATE_FUNC(TextWithBackground);
        void setPosition(const cocos2d::Vec2& position) override;
        void setContentSize(const cocos2d::Size &contentSize) override;
        void SetTextColor(cocos2d::Color3B c);
    };
    class TextInputWithBackground : public ControlWithBackground
    {
        cocos2d::ui::TextField *text;
        void InitWithContent(int ctrlID, cocos2d::Rect r, std::string text, std::string fontName, int fontSize);
    public:
        static TextInputWithBackground *CreateText(int ctrlID, cocos2d::Rect r, std::string text, std::string fontName, int fontSize);
        void AddEventListener(cocos2d::ui::TextField::ccTextFieldCallback callback);
        void SetText(std::string s);
        CREATE_FUNC(TextInputWithBackground);
    };
    class Toolbar : public cocos2d::ui::Layout
    {
        struct ToolbarButton{
            cocos2d::ui::Button* button;
            std::string bitmap;
            std::string checkedBitmap;
            ToolbarButton(cocos2d::ui::Button* _button, std::string _bitmap ,std::string _checkedBitmap):
            button(_button), bitmap(_bitmap), checkedBitmap(_checkedBitmap) {}
            void SetChecked(bool checked)
            {
                std::string image=checked?checkedBitmap:bitmap;
                button->loadTextureNormal(image);
            }
            cocos2d::Size GetSize()
            {
                return button->getContentSize();
            }
            void SetPosition(cocos2d::Vec2 pos)
            {
                return button->setPosition(pos);
            }
        };
        std::vector<ToolbarButton*> buttons;
        float xOffset, yOffset;
        
        ToolbarButton *GetButton(int ctrlID);
        void SetOffsets(float _xOffset, float yOffset);
        void UpdateLayout();
        
    public:
        ~Toolbar();
        static Toolbar *CreateToolbar(cocos2d::Rect r, float xOffset, float yOffset);
        void AddButton(int ctrlID, cocos2d::Size s, std::vector<std::string> images, Widget::ccWidgetTouchCallback callback);
        void CheckButton(int ctrlID);
        void RemoveButton(int ctrlID);
        CREATE_FUNC(Toolbar);
    };
}
#endif /* defined(__ScdfController__Label__) */
