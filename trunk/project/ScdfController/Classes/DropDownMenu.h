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
        bool hidden;
        DropDownMenuData(std::string _text, cocos2d::Color3B _c) : text(_text), c(_c), hidden(false) {}
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
        bool opened, resizeParent, touchEnabled;
        float parentHeightWithoutMenu;
        DropDownMenuCallback *callback;
        int lastSelectedIndex;
        void ScrollToSelected();
        void Resize();
        void DoResizeAndScroll(float newHeight, bool disableScrolling);
        bool ToggleOpenMenu();
        void BeforeInitData();
        void AfterInitData();
        void InitNullData();
        virtual float GetVerticalMargin() {return 0;}
        void EnableTouchEvents(bool enable) { touchEnabled=enable;}
        int GetNumItems();
        virtual Widget *CreateElement(int dataIndex);
        int GetMenuIndexFromDataIndex(int itemDataIndex);
    protected:
        float itemHeight;
        DropDownMenu();
        std::vector<DropDownMenuData> itemsData;
        void DoInitData();
        virtual void OnToggleOpenMenu();
        void handleMoveLogic(cocos2d::Touch *touch) override;
        void handleReleaseLogic(cocos2d::Touch *touch) override;
    public:
        void OnControlTouch(Ref *pSender, cocos2d::ui::ListView::EventType type);
        int GetSelectedIndex();
        void SetSelectedIndex(int selected);
        void InitData(std::vector<DropDownMenuData> data, float itemHeight);
        void setPosition(const cocos2d::Vec2 &pos) override;
        //void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        void SetCallback(DropDownMenuCallback *_callback);
        void updateTweenAction(float value, const std::string& key);
        ~DropDownMenu();
        template <class DropDownType> static DropDownMenu *CreateMenu(cocos2d::Size s, DropDownMenuCallback *callback);
        void HideItem(int itemIndex, bool hide);
        void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
        void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;

        void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
        CREATE_FUNC(DropDownMenu);
    };
    class DropDownColorMenu : public DropDownMenu
    {
        DropDownColorMenu() : DropDownMenu() {}
        float GetVerticalMargin() override {return itemHeight/2.0;}
        Widget *CreateElement(int dataIndex) override;
    protected:
        void OnToggleOpenMenu(){}
    public:
        CREATE_FUNC(DropDownColorMenu);
    };
}
#endif /* defined(__ScdfController__DropDownMenu__) */
