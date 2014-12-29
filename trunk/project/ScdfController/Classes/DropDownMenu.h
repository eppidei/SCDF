//
//  DropDownMenu.h
//  ScdfController
//
//  Created by Andrea Scuderi on 29/12/14.
//
//

#ifndef __ScdfController__DropDownMenu__
#define __ScdfController__DropDownMenu__

#include <stdio.h>

#include "SCDFCDefinitions.h"
#include "Observer.h"

namespace cocos2d{
    namespace ui{
        class ListView;
        class Text;
    }
}

namespace SCDFC
{
    class DropDownMenu;
    class DropDownMenuCallback
    {
    public:
        DropDownMenuCallback(){}
        virtual void OnSizeChanged(float oldSize, float newSize) = 0;
        virtual void OnSelectItem(DropDownMenu *menu) = 0;
    };

    class DropDownMenu : public cocos2d::ui::ListView, public cocos2d::ActionTweenDelegate
    {
        bool opened, showLabel, resizeParent;
        float parentHeightWithoutMenu;
        DropDownMenuCallback *callback;
        int selectedIndex;
        
        void OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type);
        void ScrollToSelected();
        void ResizeAndScroll(float newHeight, bool disableScrolling);
        void ToggleOpenMenu();
        DropDownMenu();
    public:
        void SetSelectedIndex(int selected);
        void InitData(std::vector<std::string> data);
        void SetMenuPosition(cocos2d::Vec2 pos);
        void SetCallback(DropDownMenuCallback *_callback);
        void updateTweenAction(float value, const std::string& key);
        ~DropDownMenu();
        static DropDownMenu *CreateMenu(cocos2d::Size s);
        CREATE_FUNC(DropDownMenu);
    };
}
#endif /* defined(__ScdfController__DropDownMenu__) */
