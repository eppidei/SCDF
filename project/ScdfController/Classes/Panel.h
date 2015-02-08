//
//  Panel.h
//  ScdfController
//
//  Created by Andrea Scuderi on 08/02/15.
//
//

#ifndef __ScdfController__Panel__
#define __ScdfController__Panel__

#include <stdio.h>
#include "SCDFCDefinitions.h"
#include "DropDownMenu.h"

#define SUBPANEL_DISTANCE 20
#define SUBPANEL_ITEM_HEIGHT 45.0

namespace ScdfCtrl
{
    class MainScene;
    
    class PanelBase : public cocos2d::ui::ScrollView
    {
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void PlaceSubPanels();
        void CalculateInnerHeight();
        void UpdateAll();
        virtual void InitPanel(){}
    public:
        void InitLayout();
        void HideShow();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        template <class PanelType> static PanelBase *CreatePanel(MainScene *main, cocos2d::Rect r);
        void EnableScrolling(bool enable);
        CREATE_FUNC(PanelBase);
    };
    
    
    class SubpanelBase : public cocos2d::ui::Layout, public cocos2d::ActionTweenDelegate
    {
        friend class PropertiesPanel;
        class DropDownMenuCallbackSubPanel : public DropDownMenuCallback
        {
            SubpanelBase *parent;
        public:
            DropDownMenuCallbackSubPanel(SubpanelBase *_parent) : parent(_parent) {}
            void OnSizeChanged(float oldSize, float newSize);
            void OnSelectItem(DropDownMenu *menu);
        };
//        class SubPanelItemCallback : public ItemBaseCallback
//        {
//            SubpanelBase *parent;
//        public:
//            SubPanelItemCallback(SubpanelBase *_parent) : parent(_parent) {}
//            void OnItemTouchBegan();
//            void OnItemTouchMoved(int value);
//            void OnItemTouchEnded();
//        };
        
        std::unique_ptr<PanelBase> parent;
        void Resize(float newHeight);
        void CalculateHeight();
        void HideElement(Node *n, bool hide);
        virtual void CreateControls() = 0;
    protected:
        std::unique_ptr<DropDownMenuCallbackSubPanel> dropDownCallback;
       // std::unique_ptr<SubPanelItemCallback> itemCallback;
        
        void TextFieldEventCallback(Ref *pSender, cocos2d::ui::TextField::EventType type);
        void TouchEventCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void CheckBoxEventCallback(Ref* pSender,cocos2d::ui::CheckBox::EventType type);
        virtual void OnCheckEvent(cocos2d::ui::CheckBox *widget, bool selected){}
        virtual void OnTextInput(cocos2d::ui::TextField *widget){}
        virtual void OnTouchEventBegan(cocos2d::ui::Button *widget){}
        
        virtual void InitChildrensVisibilityAndPos();
    public:
        ~SubpanelBase();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        virtual void PositionElements() = 0;
        virtual void OnDropDownSelectionChange(DropDownMenu *menu) {}
        virtual void UpdateValues() = 0;
        PanelBase *GetParent();
        void updateTweenAction(float value, const std::string& key) override;
        void InitWithContent(PanelBase *parent, cocos2d::Size s);
    };
};

#endif /* defined(__ScdfController__Panel__) */
