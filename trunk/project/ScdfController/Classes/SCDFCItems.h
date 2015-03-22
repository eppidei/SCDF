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
//#include "MultiSender.h"
#include "ControlUnit.h"
#include "ScdfSensorAPI.h"

namespace ScdfCtrl
{
	class ControlUnit;
	class ControlUnitSlider;
	class ControlUnitPad;
	class ControlUnitDsp;
	class MultiSender;
    class ItemBase;

    class ItemBaseCallback
    {
    public:
        virtual void OnItemTouchBegan() = 0;
        virtual void OnItemTouchMoved(int value) = 0;
        virtual void OnItemTouchEnded() = 0;
    };

	class ItemProperties {

	};

    class ItemLayoutInterface
    {
    public:
        virtual cocos2d::Size CalculateNewItemSize(int magValue) = 0;
        virtual void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) = 0;
        virtual void SetItemContentSize(const cocos2d::Size &contentSize) = 0;
    };
    class ItemLayoutManager
    {
        ItemBase *item;
        int magValue;
        bool isVertical;
        cocos2d::Size GetSizeConsideringOrientation(const cocos2d::Size resized);
        bool CheckNewSizeCollision(cocos2d::Rect r);
    public:
        ItemLayoutManager(ItemBase *parent) : magValue(0), item(parent) {}
        void SetVertical(bool vertical);
        bool IsVertical() {return isVertical;}
        void IncrementInflate();
        void DecrementInflate();
        int GetMagValue() { return magValue;}
    };
    
    class ItemBase : public /*cocos2d::Sprite*/cocos2d::ui::Layout, public SubjectSimple, public ItemLayoutInterface
    {
    protected:

        std::unique_ptr<ControlUnit> controlUnit;
        std::unique_ptr<ItemLayoutManager> layoutManager;

        std::string name;
        //cocos2d::Color3B color;
        Colors::ItemsColorsId colorIndex;
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        std::unique_ptr<ItemBaseCallback> callback;
        virtual void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type){}
    
    public:

        ControlUnit* GetControlUnit() { return controlUnit.get(); };
        ItemLayoutManager* GetLayoutManager() { return layoutManager.get(); };
        void ChangeControlUnit(ControlUnit::Type t);

        static ItemBase *CreateItem(cocos2d::Rect r, int itemID);
        //static ItemBase *CreateItemFromControlUnit(ControlUnit* cu);

        void ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        ItemBase();
        virtual ~ItemBase();
        virtual void Create()=0;
        //void SetCallback(ItemBaseCallback *c) {callback.reset(c);}


        std::string GetName();
        Colors::ItemsColorsId GetColor();
        virtual int GetValue();
        ScdfCtrl::MultiSender *GetSender();
        void SetName(std::string _name);
        virtual void SetColor(Colors::ItemsColorsId _colorIndex);
      //  virtual void SetValue(int _value);

        virtual cocos2d::Size GetStaticBaseSize()=0;
        void LaunchCollisionAnimation();
    };

    class ItemSlider : public ItemBase
    {
        
        virtual void SetPositionOfValueDependentComponent();
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
//        bool isVertical;
        //float min, max;
        void CreateThumb();
//        void LinearMode(cocos2d::Vec2 touchPos);
    public:
        virtual void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return SLIDER_SIZE_BASE;}
        static int GetID() { return ITEM_SLIDER_ID;}
        static std::string GetIcon() { return "iconSliderDefault.png";}
        static std::string GetIconPressed() { return "iconSliderPressed.png";}
//        void SetRange(int _min, int _max);
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        cocos2d::Size CalculateNewItemSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override;
        void SetItemContentSize(const cocos2d::Size &contentSize) {setContentSize(contentSize);}
        CREATE_FUNC(ItemSlider);
    };

    class ItemKnob : public ItemSlider
    {
        void SetPositionOfValueDependentComponent() override;
        bool IsKnob() { return true;}
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        void DoCreateThumb() override;
        cocos2d::Size GetThumbSize(cocos2d::Size currentSize) override;
        
        cocos2d::ui::Layout *knob, *points;
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
        static std::string GetIcon() { return "iconKnobDefault.png";}
        static std::string GetIconPressed() { return "iconKnobPressed.png";}
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        cocos2d::Size CalculateNewItemSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void SetItemContentSize(const cocos2d::Size &contentSize) {setContentSize(contentSize);}
        
        CREATE_FUNC(ItemKnob);
    };


    class ItemKeyboard;
    class ItemPad : public ItemBase
    {
        friend class ItemMultipad;
        cocos2d::ui::Button *pad;
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }

    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        //int midiNote;
        void Create();
        static cocos2d::Size GetBaseSize() { return PAD_SIZE_BASE;}
        static int GetID() { return ITEM_PAD_ID;}
        static std::string GetIcon() { return "iconPadDefault.png";}
        static std::string GetIconPressed() { return "iconPadPressed.png";}
        
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        cocos2d::Size CalculateNewItemSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void SetItemContentSize(const cocos2d::Size &contentSize) {setContentSize(contentSize);}
        
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

        std::shared_ptr<ControlUnitPad> ctrlUnitPad;

        public:

		ItemPad *GetSelectedPad();
        void UpdateSelectedPadIndex(ItemPad *pad);
        void Create();
        static cocos2d::Size GetBaseSize() { return MULTIPAD_SIZE_BASE;}
        static int GetID() { return ITEM_MULTIPAD_ID;}
        
        cocos2d::Size CalculateNewItemSize(int magValue) override {return getContentSize();}
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void SetItemContentSize(const cocos2d::Size &contentSize) {setContentSize(contentSize);}
        
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
        static std::string GetIcon() { return "iconPianoDefault.png";}
        static std::string GetIconPressed() { return "iconPianoPressed.png";}
        
//        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        cocos2d::Size CalculateNewItemSize(int magValue) override {return getContentSize();};
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void SetItemContentSize(const cocos2d::Size &contentSize) override {}
        
        CREATE_FUNC(ItemKeyboard);
    };
    class ItemSwitch : public ItemBase
    {
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }

        ControlUnitDsp* ctrlUnitDsp;

    public:

        void Create(){}
        static cocos2d::Size GetBaseSize() { return RECT_SENSOR_SIZE_BASE;}
        static int GetID() { return ITEM_SWITCH_ID;}
        static std::string GetIcon() { return "iconVuMeterDefault.png";}
        static std::string GetIconPressed() { return "iconVuMeterPressed.png";}
        
        cocos2d::Size CalculateNewItemSize(int magValue) override {return getContentSize();};
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void SetItemContentSize(const cocos2d::Size &contentSize) override {}
        
        CREATE_FUNC(ItemSwitch);
    };
}

#endif /* defined(__ScdfController__SCDFCItems__) */
