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
#include "ControlUnit.h"
#include "ScdfSensorAPI.h"
#include "Label.h"



namespace ScdfCtrl
{
    class ItemBase;
    class SerializableItemData;
    class ItemUIUpdater;
    
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
    
    class ItemBase : public cocos2d::ui::Layout, public SubjectSimple, public ItemLayoutInterface
    {
        TextWithBackground *label;
        Layout *controlImage;
        
        void CreateItemBaseElements();
        void PlaceItemBaseElements();
        void SetControlModeImage();
        std::unique_ptr<ItemUIUpdater> updater;
        
        virtual void SetControlUnitReceiverType() {}
    protected:

        std::unique_ptr<ControlUnit> controlUnit;
        std::unique_ptr<ItemLayoutManager> layoutManager;
        std::string name;
        Colors::ItemsColorsId colorIndex;
        int groupId;
        bool isMaster;
        
        cocos2d::ui::Layout *control;
        cocos2d::Vec2 dragStartPos, dragPosUpdated;
        //std::unique_ptr<ItemBaseCallback> callback;
        
        cocos2d::Size GetControlContentSize();
        virtual void DoSetContentSize(cocos2d::Size contentSize) = 0;

        void SetControlUnit(ControlUnit* cu); // only for deserializeitem use

    public:

        virtual bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual bool OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        virtual bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type);
        template <class ItemType> static ItemBase *CreateItem();
        static ItemBase* DeserializeItem(SerializableItemData* sitem);
        
        ItemBase();
        virtual ~ItemBase();
        virtual void Create()=0;
        static ItemBase* CreateItem(int id);

        void LaunchCollisionAnimation();
        void ItemsTouchCallback(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void Select(bool select);
        
        //setter
        void ChangeControlUnit(ControlUnit::Type t);
        void SetName(std::string _name);
        void SetGroupID(int _groupId);
        void SetMaster(bool _isMaster);
        virtual void SetColor(Colors::ItemsColorsId _colorIndex);
        virtual void setContentSize(const cocos2d::Size &contentSize) override;
        
        //getter
        ControlUnit* GetControlUnit() { return controlUnit.get(); };
        ItemLayoutManager* GetLayoutManager() { return layoutManager.get(); };

        std::string GetName();
        Colors::ItemsColorsId GetColor();
        int GetGroupID();
        bool IsMaster();

        virtual cocos2d::Size GetStaticBaseSize()=0;
        virtual int GetID()=0;
        
        virtual void UpdateUI() = 0;
        
    };

    class ItemSlider : public ItemBase
    {
        virtual bool IsKnob() { return false;}
        bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        bool OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        cocos2d::Size GetStaticBaseSize() override;
        virtual void DoCreateThumb();
        virtual cocos2d::Size GetThumbSize(cocos2d::Size currentSize);
        virtual cocos2d::Vec2 OnMove(cocos2d::ui::Widget *widget);
        virtual void Init();
        
        virtual bool IsPitchWheel() { return false;}
        void SetControlUnitReceiverType() { GetControlUnit()->SetReceiverType(ControlUnit::ReceiverType_stream);}
    protected:
        cocos2d::ui::Layout *thumb;
        cocos2d::ui::Layout *slideBar;
        cocos2d::ui::Layout *slideBarOff;
        
        void CreateThumb();
        virtual void SetPositionOfValueDependentComponent();
        virtual bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
//        void LinearMode(cocos2d::Vec2 touchPos);
        void UpdateUI() override;
    public:
        virtual void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return SLIDER_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID() { return ITEM_SLIDER_ID;}
        static std::string GetIcon() { return "iconSliderDefault.png";}
        static std::string GetIconPressed() { return "iconSliderPressed.png";}
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
        
        cocos2d::ui::Layout *knob;//, *points;
        std::vector<cocos2d::ui::Layout*> onPoints;
        void CreateONPoints();
        void UpdateOnPointVisibility();
        void AngularMode(Widget *knob);
        cocos2d::Vec2 OnMove(cocos2d::ui::Widget *widget) override;
    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        void Create();
        static cocos2d::Size GetBaseSize() { return KNOB_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID()  { return ITEM_KNOB_ID;}
        static std::string GetIcon() { return "iconKnobDefault.png";}
        static std::string GetIconPressed() { return "iconKnobPressed.png";}
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        
        CREATE_FUNC(ItemKnob);
    };
    
    class ItemWheel : public ItemSlider
    {
        virtual bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        void Init() override;
        bool IsPitchWheel() override{ return true;}
    public:
        
        static cocos2d::Size GetBaseSize() { return WHEEL_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID() { return ITEM_WHEEL_ID;}
        static std::string GetIcon() { return "iconPitchWheelDefault.png";}
        static std::string GetIconPressed() { return "iconPitchWheelPressed.png";}
        
        CREATE_FUNC(ItemWheel);
    };


    class ItemKeyboard;
    class ItemPad : public ItemBase
    {
        friend class ItemMultipad;
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        
        virtual void Init();
        
        void SetControlUnitReceiverType() { GetControlUnit()->SetReceiverType(ControlUnit::ReceiverType_state);}
    protected:
        cocos2d::ui::Button *pad;
        
        virtual bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        virtual bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        virtual void UpdateUI() override;
    public:
        void SetColor(Colors::ItemsColorsId colorIndex) override;
        //int midiNote;
        void Create();
        static cocos2d::Size GetBaseSize() { return PAD_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID() { return ITEM_PAD_ID; }
        static std::string GetIcon() { return "iconPadDefault.png";}
        static std::string GetIconPressed() { return "iconPadPressed.png";}
        
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override;
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override {}
        
        CREATE_FUNC(ItemPad);
    };
    class ItemSwitch : public ItemPad
    {
        bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        void Init() override;
        bool IsChecked();
        bool checked;
        
        void UpdateUI() override;
        cocos2d::ui::Layout *backGround;
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        
        void SetControlUnitReceiverType() { GetControlUnit()->SetReceiverType(ControlUnit::ReceiverType_toggle);}
    public:
        static cocos2d::Size GetBaseSize() { return SWITCH_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID() { return ITEM_SWITCH_ID;}
        static std::string GetIcon() { return "iconSwitchDefault.png";}
        static std::string GetIconPressed() { return "iconSwitchPressed.png";}
        void DoSetContentSize(cocos2d::Size contentSize) override;
        
        CREATE_FUNC(ItemSwitch);
    };
    
    class ItemMultipad: public ItemBase
    {
        int padSizeMultiply;
        std::vector<ItemPad*> pads;
        int padIndex;
        void ClearPads();
        void CreatePads();
        bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }

        void UpdateUI() override {};
    public:

		ItemPad *GetSelectedPad();
        void UpdateSelectedPadIndex(ItemPad *pad);
        void Create();
        static cocos2d::Size GetBaseSize() { return MULTIPAD_SIZE_BASE;}
        int GetID() override { return ID();}
        static int ID() { return ITEM_MULTIPAD_ID;}
        
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
        
        bool OnItemTouchBegan(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        bool OnItemTouchEnded(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        bool OnItemTouchMoved(cocos2d::ui::Widget* widget, cocos2d::ui::Widget::TouchEventType type) override;
        cocos2d::Size GetStaticBaseSize() override { return GetBaseSize(); }
        bool UpdateSelectedKey(cocos2d::ui::Widget* widget, bool onMoving);
        
        void UpdateUI() override {};
    public:
        void SetCurrentOctave(int octave) { currentOctave=octave; }
        int GetCurrentOctave() {return currentOctave;}
        void Create();
        static cocos2d::Size GetBaseSize() { return KEYBOARD_SIZE_BASE;}    //for dragging
        int GetID() override { return ID();}
        static int ID() { return ITEM_KEYBOARD_ID;}                      //for item scrollbar
        static std::string GetIcon() { return "iconPianoDefault.png";}          //for item scrollbar
        static std::string GetIconPressed() { return "iconPianoPressed.png";}   //for item scrollbar
    
        void DoSetContentSize(cocos2d::Size contentSize) override;
        cocos2d::Size CalculateNewItemBaseSize(int magValue) override {return GetStaticBaseSize();}
        void InitLayoutOrientation(cocos2d::Vec2 rotationCenter) override;
        
        CREATE_FUNC(ItemKeyboard);
    };

    class ItemUIUpdater :public ControlUnitInterface
    {
        ItemBase *item;
    public:
        ItemUIUpdater(ItemBase *_item) : item(_item) {}
        void OnValueChanged() override { item->UpdateUI();}
    };
}

#endif /* defined(__ScdfController__SCDFCItems__) */
