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


namespace ScdfCtrl
{
    struct DropDownMenuData
    {
        std::string text;
        cocos2d::Color3B color;
        DropDownMenuData(std::string _text, cocos2d::Color3B _color) : text(_text), color(_color) {}
    };
    
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
        bool opened, resizeParent;
        float parentHeightWithoutMenu;
        DropDownMenuCallback *callback;
        int lastSelectedIndex;
        std::vector<DropDownMenuData> itemsData;
        void OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type);
        void ScrollToSelected();
        void ResizeAndScroll(float newHeight, bool disableScrolling);
        void ToggleOpenMenu();
        void BeforeInitData(float itemHeight);
        void AfterInitData(int numElements);
        bool CheckDataSize(std::vector<DropDownMenuData> data);
    protected:
        DropDownMenu();
        virtual void DoInitData(std::vector<DropDownMenuData> data);
    public:
        DropDownMenuData GetSelectedItemInfo();
        void SetSelectedIndex(int selected);
        void InitData(std::vector<DropDownMenuData> data, float itemHeight);
        void setPosition(const cocos2d::Vec2 &pos) override;
        void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        void SetCallback(DropDownMenuCallback *_callback);
        void updateTweenAction(float value, const std::string& key);
        ~DropDownMenu();
        template <class DropDownType> static DropDownMenu *CreateMenu(cocos2d::Size s);
        CREATE_FUNC(DropDownMenu);
    };
    class DropDownColorMenu : public DropDownMenu
    {
        DropDownColorMenu() : DropDownMenu() {}
    protected:
        void DoInitData(std::vector<DropDownMenuData> data) override;
    public:
        CREATE_FUNC(DropDownColorMenu);
    };
}
#endif /* defined(__ScdfController__DropDownMenu__) */
