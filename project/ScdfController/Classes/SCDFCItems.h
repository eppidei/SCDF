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
#include "Colors.h"

namespace ScdfCtrl
{
    class ControlUnit;
    
    class ItemBaseCallback
    {
    public:
        virtual void OnItemTouchBegan() = 0;
        virtual void OnItemTouchMoved(int value) = 0;
        virtual void OnItemTouchEnded() = 0;
    };
    class ItemBase : public cocos2d::Sprite/*ui::Layout*/, public SubjectSimple
    {
        int inflateHValue;
        int inflateVValue;
        void CalculateItemSize(float gridBase);
    protected:
        int value;
        std::string name;
        //cocos2d::Color3B color;
        Colors::ItemsColorsId colorIndex;
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        std::unique_ptr<ScdfCtrl::ControlUnit> controlUnit;
        std::unique_ptr<ItemBaseCallback> callback;
        virtual void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type){}
    public:
        static ItemBase *CreateItem(cocos2d::Rect r, int itemID);
        void ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        ScdfCtrl::ControlUnit *GetControlUnit() { return controlUnit.get();}
        ItemBase();
        virtual void Create()=0;
        virtual ~ItemBase(){}
        void SetCallback(ItemBaseCallback *c) {callback.reset(c);}
        void SetName(std::string _name) {name=_name;}
        virtual void SetColor(Colors::ItemsColorsId _colorIndex) {colorIndex=_colorIndex;}
        void SetValue(int _value) {value=_value;}
        void IncrementInflate(bool isHeight);
        void DecrementInflate(bool isHeight);
        std::string GetName() {return name;}
        int GetColor() {return colorIndex;}
        int GetInflateValue(bool height) {if (height) return inflateVValue; return inflateHValue;}
        virtual int GetValue() {return value;}
        virtual cocos2d::Size GetStaticBaseSize()=0;
    };
    class ItemSlider : public ItemBase
    {
        cocos2d::Size GetSizeConsideringOrientation(const cocos2d::Size resized);
        virtual void SetThumbPosition();
        virtual bool IsKnob() { return false;}
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override;
        virtual void DoCreateThumb();
        virtual cocos2d::Size GetThumbSize(cocos2d::Size currentSize);
        virtual cocos2d::Vec2 OnMove(cocos2d::ui::Widget *widget);
    protected:
        cocos2d::ui::Layout *thumb;
        cocos2d::ui::Layout *slideBar;
        cocos2d::ui::Layout *slideBarOff;
        bool isVertical;
        float min, max;
        void CreateThumb();
        void LinearMode(cocos2d::Vec2 touchPos);
    public:
        virtual void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return SLIDER_SIZE_BASE;}
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
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        void DoCreateThumb() override;
        cocos2d::Size GetThumbSize(cocos2d::Size currentSize) override;
        std::vector<cocos2d::ui::Layout*> onPoints;
        void CreateONPoints();
        void UpdateOnPointVisibility();
        void AngularMode(cocos2d::Vec2 touchPos);
        cocos2d::Vec2 OnMove(cocos2d::ui::Widget *widget) override;
    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return KNOB_SIZE_BASE;}
        static int GetID() { return ITEM_KNOB_ID;}
        CREATE_FUNC(ItemKnob);
    };
    class ItemPad : public ItemBase
    {
        friend class ItemMultipad;
        cocos2d::ui::Button *pad;
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        int midiNote;
        void Create();
        static cocos2d::Size GetBaseSize() { return PAD_SIZE_BASE;}
        static int GetID() { return ITEM_PAD_ID;}
        ~ItemPad();
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        CREATE_FUNC(ItemPad);
    };
    class ItemMultipad: public ItemBase
    {
        int padSizeMultiply;
        std::vector<ItemPad*> pads;
        int padIndex;
        void ClearPads();
        void CreatePads();
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
    public:
        ItemPad *GetSelectedPad();
        void UpdateSelectedPadIndex(ItemPad *pad);
        void Create();
        static cocos2d::Size GetBaseSize() { return MULTIPAD_SIZE_BASE;}
        static int GetID() { return ITEM_MULTIPAD_ID;}
        ~ItemMultipad();
        CREATE_FUNC(ItemMultipad);
    };
    class ItemKeyboard : public ItemBase
    {
        cocos2d::ui::Layout *keysHandle;
        cocos2d::Sprite *blackKeyPressed;
        cocos2d::Sprite *whiteKeyPressed;
        int selectedKey;
        int currentOctave;
        
        float topBitmapOffsetPercentageForPressedKey;
        float leftBitmapOffsetPercentageForPressedKey;
        
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        void UpdateSelectedKey();
    public:
        //void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        void SetCurrentOctave(int octave) { currentOctave=octave; }
        int GetCurrentOctave() {return currentOctave;}
        void Create();
        static cocos2d::Size GetBaseSize() { return KEYBOARD_SIZE_BASE;}
        static int GetID() { return ITEM_KEYBOARD_ID;}
        ~ItemKeyboard();
        CREATE_FUNC(ItemKeyboard);
    };
    class ItemSwitch : public ItemBase
    {
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
    public:
        void Create(){}
        static cocos2d::Size GetBaseSize() { return RECT_SENSOR_SIZE_BASE;}
        static int GetID() { return ITEM_SWITCH_ID;}
        CREATE_FUNC(ItemSwitch);
    };
}
#endif /* defined(__ScdfController__SCDFCItems__) */
