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
#define SUBPANEL_ITEM_HEIGHT 30.0

namespace ScdfCtrl
{
    class MainScene;
    class SubpanelBase;
    class PanelBase : public cocos2d::ui::Layout
    {
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void PlaceSubPanels();
        void CalculateInnerHeight();
        virtual void UpdateSubpanels(){}
        virtual void InitPanel(){}
        
        bool visible;
    public:
        void InitLayout();
        bool HideShow(PanelBase *substitute=NULL);
     //   virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        template <class PanelType> static PanelBase *CreatePanel(MainScene *main, cocos2d::Rect r);
        void EnableScrolling(bool enable);
        void CollapseAllSubpanelsButThis(SubpanelBase *subPanel);
        bool IsVisible() { return visible;}
        CREATE_FUNC(PanelBase);
    protected:
        cocos2d::ui::ScrollView *scrollView;
        MainScene *parent;
    };
    
    
    class SubpanelBase : public cocos2d::ui::Layout, public cocos2d::ActionTweenDelegate
    {
        class DropDownMenuCallbackSubPanel : public DropDownMenuCallback
        {
            SubpanelBase *parent;
        public:
            DropDownMenuCallbackSubPanel(SubpanelBase *_parent) : parent(_parent) {}
            void OnSizeChanged(float oldSize, float newSize);
            void OnSelectItem(DropDownMenu *menu);
        };
        std::unique_ptr<PanelBase> parent;
        
        void Resize(float newHeight);
        void CalculateHeight();
        virtual void CreateControls() = 0;
        virtual int GetYPadding() = 0;
    protected:
        std::unique_ptr<DropDownMenuCallbackSubPanel> dropDownCallback;
        bool collapsed;
        
        void HideElement(Node *n, bool hide);
        void EnableElement(Widget *w, bool enable);
        virtual void OnCheckEvent(cocos2d::ui::CheckBox *widget, bool selected){}
        virtual void OnTextInput(cocos2d::ui::TextField *widget){}
        virtual void OnTouchEventBegan(cocos2d::Node *widget){}
        virtual void OnTouchEventEnded(cocos2d::Node *widget){}
        virtual void OnListViewItemSelected(cocos2d::ui::ListView *widget){}
        virtual void CheckShowElements() {}
        void UpdateLayout();
    public:
        ~SubpanelBase();
//        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        virtual void PositionElements() = 0;
        virtual void OnDropDownSelectionChange(DropDownMenu *menu) {}
        virtual void Update() {}
        PanelBase *GetParent();
        void updateTweenAction(float value, const std::string& key) override;
        void InitWithContent(PanelBase *parent, cocos2d::Size s);
        
        void TextFieldEventCallback(Ref *pSender, cocos2d::ui::TextField::EventType type);
        void TouchEventCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void CheckBoxEventCallback(Ref* pSender,cocos2d::ui::CheckBox::EventType type);
        void ListviewItemEvent(Ref *pSender, cocos2d::ui::ListView::EventType type);
        bool IsCollapsed() { return collapsed;}
        void SetCollapsed(bool collapsed);
    };
};

#endif /* defined(__ScdfController__Panel__) */
