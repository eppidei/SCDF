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
        CREATE_FUNC(TextWithBackground);
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
}
#endif /* defined(__ScdfController__Label__) */
