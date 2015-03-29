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
#include "Label.h"

namespace ScdfCtrl
{
//	class ControlUnit;
//	class ControlUnitSlider;
//	class ControlUnitPad;
//	class ControlUnitDsp;
//	class MultiSender;
    class ItemBase;

//    class ItemBaseCallback
//    {
//    public:
//        virtual void OnItemTouchBegan() = 0;
//        virtual void OnItemTouchMoved(int value) = 0;
//        virtual void OnItemTouchEnded() = 0;
//    };

	class ItemProperties {

	};

    class ItemLayoutInterface
    {
    public:
        virtual cocos2d::Size CalculateNewItemBaseSize(int magValue) = 0;
        virtual void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) = 0;
    };
    class ItemLayoutManager
    {
        ItemBase *item;
        int magValue;
        bool isVertical;
        cocos2d::Size GetSizeConsideringOrientation(const cocos2d::Size resized);
        bool CheckNewSizeCollision(cocos2d::Rect r);
        cocos2d::Size CalculateNewSize(int _magValue);
    public:
        ItemLayoutManager(ItemBase *parent) : magValue(0), item(parent) {}
        void SetVertical(bool vertical);
        bool IsVertical() {return isVertical;}
        bool ControlIconOnTop(cocos2d::Size itemBaseSize);
        void ZoomPlus();
        void ZoomMinus();
        void SetMagValue(int magValue);
        int GetMagValue() { return magValue;}
    };
    
    class ItemBase : public /*cocos2d::Sprite*/cocos2d::ui::Layout, public SubjectSimple, public ItemLayoutInterface
    {
        TextWithBackground *label, *label1;
        cocos2d::Sprite *controlImage;
        
        void CreateItemBaseElements();
        void PlaceItemBaseElements();
    protected:

        std::unique_ptr<ControlUnit> controlUnit;
        std::unique_ptr<ItemLayoutManager> layoutManager;
        std::string name;
        Colors::ItemsColorsId colorIndex;
        
        cocos2d::ui::Layout *control;
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        //std::unique_ptr<ItemBaseCallback> callback;

        virtual void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type){}
        
        cocos2d::Size GetControlContentSize();
        virtual void DoSetContentSize(cocos2d::Size contentSize) = 0;
    public:

        template <class ItemType> static ItemBase *CreateItem();
        
        ItemBase();
        virtual ~ItemBase();
        virtual void Create()=0;

        void LaunchCollisionAnimation();
        void ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        
        //setter
        void ChangeControlUnit(ControlUnit::Type t);
        void SetName(std::string _name);
        virtual void SetColor(Colors::ItemsColorsId _colorIndex);
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        //getter
        ControlUnit* GetControlUnit() { return controlUnit.get(); };
        ItemLayoutManager* GetLayoutManager() { return layoutManager.get(); };

        std::string GetName();
        Colors::ItemsColorsId GetColor();

        virtual cocos2d::Size GetStaticBaseSize()=0;
//        virtual int GetStaticID()=0;
        
    };

    class ItemSlider : public ItemBase
    {
        virtual bool IsKnob() { return false;}
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        cocos2d::Size GetStaticBaseSize() override;
        virtual void DoCreateThumb();
        virtual cocos2d::Size GetThumbSize(cocos2d::Size currentSize);
        virtual cocos2d::Vec2 OnMove(cocos2d::ui::Widget *widget);
        virtual void Init();
    protected:
        cocos2d::ui::Layout *thumb;
        cocos2d::ui::Layout *slideBar;
        cocos2d::ui::Layout *slideBarOff;
//        bool isVertical;
        //float min, max;
        void CreateThumb();
        virtual void SetPositionOfValueDependentComponent();
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
//        void LinearMode(cocos2d::Vec2 touchPos);
    public:
        virtual void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return SLIDER_SIZE_BASE;}
        static int GetID() { return ITEM_SLIDER_ID;}
        static std::string GetIcon() { return "iconSliderDefault.png";}
        static std::string GetIconPressed() { return "iconSliderPressed.png";}
//        void SetRange(int _min, int _max);
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override;
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
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        
        CREATE_FUNC(ItemKnob);
    };
    
    class ItemWheel : public ItemSlider
    {
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        void Init() override;
    public:
        
        static cocos2d::Size GetBaseSize() { return WHEEL_SIZE_BASE;}
        static int GetID() { return ITEM_WHEEL_ID;}
        static std::string GetIcon() { return "iconSliderDefault.png";}
        static std::string GetIconPressed() { return "iconSliderPressed.png";}
        
        CREATE_FUNC(ItemWheel);
    };


    class ItemKeyboard;
    class ItemPad : public ItemBase
    {
        friend class ItemMultipad;
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        
        virtual void Init();
    protected:
        cocos2d::ui::Button *pad;
        
        virtual void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        //int midiNote;
        void Create();
        static cocos2d::Size GetBaseSize() { return PAD_SIZE_BASE;}
        static int GetID() { return ITEM_PAD_ID;}
        static std::string GetIcon() { return "iconPadDefault.png";}
        static std::string GetIconPressed() { return "iconPadPressed.png";}
        
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        
        CREATE_FUNC(ItemPad);
    };
    class ItemSwitch : public ItemPad
    {
        void OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        void Init() override;
        bool IsChecked();
        bool checked;
    public:
        static cocos2d::Size GetBaseSize() { return SWITCH_SIZE_BASE;}
        static int GetID() { return ITEM_SWITCH_ID;}
        static std::string GetIcon() { return "iconPadDefault.png";}
        static std::string GetIconPressed() { return "iconPadPressed.png";}
        
        CREATE_FUNC(ItemSwitch);
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

//        std::shared_ptr<ControlUnitPad> ctrlUnitPad;

        public:

		ItemPad *GetSelectedPad();
        void UpdateSelectedPadIndex(ItemPad *pad);
        void Create();
        static cocos2d::Size GetBaseSize() { return MULTIPAD_SIZE_BASE;}
        static int GetID() { return ITEM_MULTIPAD_ID;}
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override {return getContentSize();}
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        void DoSetContentSize(cocos2d::Size contentSize) override {}
        
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
        static cocos2d::Size GetBaseSize() { return KEYBOARD_SIZE_BASE;}    //for dragging
        static int GetID() { return ITEM_KEYBOARD_ID;}                      //for item scrollbar
        static std::string GetIcon() { return "iconPianoDefault.png";}          //for item scrollbar
        static std::string GetIconPressed() { return "iconPianoPressed.png";}   //for item scrollbar
        
//        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        void DoSetContentSize(cocos2d::Size contentSize) override;
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override {return GetStaticBaseSize();}
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        
        CREATE_FUNC(ItemKeyboard);
    };
}

#endif /* defined(__ScdfController__SCDFCItems__) */
