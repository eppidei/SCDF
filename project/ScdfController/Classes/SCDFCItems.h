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
    class ItemBaseCallback
    {
    public:
        virtual void OnItemTouchBegan() = 0;
        virtual void OnItemTouchMoved(int value) = 0;
        virtual void OnItemTouchEnded() = 0;
    };
    class ItemBase : public cocos2d::ui::Layout, public SubjectSimple
    {
        int sizeMultiply;
    protected:
        int value;
        std::string name;
        cocos2d::Color3B color;
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        std::unique_ptr<ScdfCtrl::ControlUnit> controlUnit;
        std::unique_ptr<ItemBaseCallback> callback;
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
        void SetCallback(ItemBaseCallback *c) {callback.reset(c);}
        void SetName(std::string _name) {name=_name;}
        virtual void SetColor(cocos2d::Color3B _color) {color=_color;}
        void SetValue(int _value) {value=_value;}
        void SetItemMultiply(int multiply);
        std::string GetName() {return name;}
        cocos2d::Color3B GetColor() {return color;}
        int GetSizeMultiply() {return sizeMultiply;}
        virtual int GetValue() {return value;}
    };
    class ItemSlider : public ItemBase
    {
        cocos2d::Size GetSizeConsideringOrientation(const cocos2d::Size resized);
        virtual void SetThumbPosition();
        virtual bool IsKnob() { return false;}
        void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    protected:
        Layout *thumb;
        Layout *slideBar;
        bool isVertical;
        float min, max;
        void CreateThumb();
    public:
        void Create();
        void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        static cocos2d::Size GetSize() { return SLIDER_SIZE_BASE;}
        static int GetID() { return ITEM_SLIDER_ID;}
        virtual ~ItemSlider();
        void SetVertical(bool vertical);
        void SetRange(float _min, float _max);
        void SetValue(float _value);
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
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
    class ItemKeyboard;
    class ItemPad : public ItemBase
    {
        friend class ItemKeyboard;
        cocos2d::ui::Button *pad;
        void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    public:
        int midiNote;
        void Create();
        static cocos2d::Size GetSize() { return PAD_SIZE_BASE;}
        static int GetID() { return ITEM_PAD_ID;}
        ~ItemPad();
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        CREATE_FUNC(ItemPad);
    };
    class ItemKeyboard : public ItemBase
    {
        int padSizeMultiply;
        std::vector<ItemPad*> pads;
        int padIndex;
        void ClearPads();
        void CreatePads();
        void OnItemTouchBegan(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    public:
        ItemPad *GetSelectedPad();
        void UpdateSelectedPadIndex(ItemPad *pad);
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
