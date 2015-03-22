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
#include "Colors.h"

namespace ScdfCtrl
{
    struct DropDownMenuData
    {
        cocos2d::Color3B c;
        std::string text;
        DropDownMenuData(std::string _text, cocos2d::Color3B _c) : text(_text), c(_c) {}
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
        //std::vector<DropDownMenuDataBase> itemsData;
        void OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type);
        void ScrollToSelected();
        void ResizeAndScroll(float newHeight, bool disableScrolling);
        void ToggleOpenMenu();
        void BeforeInitData(float itemHeight);
        void AfterInitData(int numElements);
        bool CheckDataSize(std::vector<DropDownMenuData> data);
        virtual float GetVerticalMargin() {return 0;}
    protected:
        DropDownMenu();
        virtual void DoInitData(std::vector<DropDownMenuData> data);
    public:
        int GetSelectedIndex();
        void SetSelectedIndex(int selected);
        void InitData(std::vector<DropDownMenuData> data, float itemHeight);
        void setPosition(const cocos2d::Vec2 &pos) override;
        //void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        void SetCallback(DropDownMenuCallback *_callback);
        void updateTweenAction(float value, const std::string& key);
        ~DropDownMenu();
        template <class DropDownType> static DropDownMenu *CreateMenu(cocos2d::Size s, DropDownMenuCallback *callback);
        CREATE_FUNC(DropDownMenu);
    };
    class DropDownColorMenu : public DropDownMenu
    {
        DropDownColorMenu() : DropDownMenu() {}
        float GetVerticalMargin() override {return getContentSize().height/2.0;}
    protected:
        void DoInitData(std::vector<DropDownMenuData> data) override;
    public:
        CREATE_FUNC(DropDownColorMenu);
    };
}
#endif /* defined(__ScdfController__DropDownMenu__) */
