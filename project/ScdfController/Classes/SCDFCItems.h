//
//  SCDFCItems.h
//  ScdfController
//
//  Created by Andrea Scuderi on 18/11/14.
//
//

#ifndef __ScdfController__SCDFCItems__
#define __ScdfController__SCDFCItems__

#include "SCDFCDefinitions.h"
#include "Observer.h"


#define MAIN_BACK_COLOR Color3B(50,50,50)


namespace ScdfCtrl
{
    class ControlUnit;
}
namespace cocos2d
{
    namespace ui
    {
        class Layout;
        class Button;
        class Widget;
    }
}
namespace SCDFC {
    class ItemBase : public cocos2d::ui::Layout, public SubjectSimple
    {
        int sizeMultiply;
    protected:
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        std::unique_ptr<ScdfCtrl::ControlUnit> controlUnit;
        virtual void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type){}
    public:
        static ItemBase *CreateItem(cocos2d::Rect r, int itemID);
        void ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        ScdfCtrl::ControlUnit *GetControlUnit() { return controlUnit.get();}
        ItemBase();
        virtual void Create()=0;
        virtual ~ItemBase(){}
        
    };
    class ItemSlider : public ItemBase
    {
        void InitSliderLayout();
        virtual void SetThumbPosition();
        virtual bool IsKnob() { return false;}
        void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    protected:
        Layout *thumb;
        Layout *slideBar;
        bool isVertical;
        float min, max, value;
        void CreateThumb();
        void DraggingControlEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
    public:
        void Create();
        static cocos2d::Size GetSize() { return SLIDER_SIZE_BASE;}
        static int GetID() { return ITEM_SLIDER_ID;}
        virtual ~ItemSlider();
        CREATE_FUNC(ItemSlider);
    };
    class ItemKnob : public ItemSlider
    {
        void SetThumbPosition();
        bool IsKnob() { return true;}
    public:
        void Create();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        static cocos2d::Size GetSize() { return KNOB_SIZE_BASE;}
        static int GetID() { return ITEM_KNOB_ID;}
        CREATE_FUNC(ItemKnob);
    };
    class ItemPad : public ItemBase
    {
        cocos2d::ui::Button *pad;
        void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    public:
        int midiNote;
        void Create();
        static cocos2d::Size GetSize() { return PAD_SIZE_BASE;}
        static int GetID() { return ITEM_PAD_ID;}
        ~ItemPad();
        CREATE_FUNC(ItemPad);
    };
    class ItemKeyboard : public ItemBase
    {
        int padSizeMultiply;
        std::vector<ItemPad*> pads;
        void ClearPads();
        void CreatePads();
    public:
        void Create();
        static cocos2d::Size GetSize() { return KEYBOARD_SIZE_BASE;}
        static int GetID() { return ITEM_KEYBOARD_ID;}
        ~ItemKeyboard();
        CREATE_FUNC(ItemKeyboard);
    };
    class ItemSensor1 : public ItemBase
    {
    public:
        void Create(){}
        static cocos2d::Size GetSize() { return RECT_SENSOR_SIZE_BASE;}
        static int GetID() { return ITEM_SENSOR1_ID;}
        CREATE_FUNC(ItemSensor1);
    };
}
#endif /* defined(__ScdfController__SCDFCItems__) */
