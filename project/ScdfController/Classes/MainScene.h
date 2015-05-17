#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "SCDFCDefinitions.h"
#include "MidiOutConnection.h"

#define PROPERTIES_PANEL_RIGHT_TRANSPARENCY_PERCENTAGE 0.085
#define SCROLLVIEW_PANEL_LEFT_TRANSPARENCY_PERCENTAGE 0.052
#define ITEMS_LABEL_HEIGHT MainScene::GetUnityBase()*2.0

namespace ScdfCtrl
{
    class WorkingPanel;
    class ItemScrollView;
    class PropertiesPanel;
    class ItemBase;
    class LoadSavePanel;
    class SerializableAppData;

    class MainScene : public cocos2d::Layer, public Scdf::MidiDeviceMenuListener
    {
        static std::vector<float> gridUnity;
        std::unique_ptr<ItemScrollView> customScrollView;
        std::unique_ptr<WorkingPanel> customPanel;
        std::unique_ptr<PropertiesPanel> propertiesPanel;

        void CalculateGrid(cocos2d::Size workingPanelSize);
        static int gridIndex;
        void OnGridButtonClick();
        void HideShowPropertiesPanel(bool hide);
    public:
        static cocos2d::Scene* createScene();
        virtual bool init();  
        
        void touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        template <class ItemType> void  OnStartDragging(cocos2d::Vec2 dragStartPoint);
        template <class ItemType> void OnDragging(cocos2d::Rect draggingRect);
        template <class ItemType> void  OnEndDragging();
        static float GetUnityBase();
        
        static int GetGridDistance();
        void SnapToGrid(cocos2d::Rect &r);
        void EnableScrollView(bool enable);
        void AttachItem(ItemBase *item);
        void DetachItem(ItemBase *item);
        void UpdateEditButton();
        void UpdateMIDIDevicesMenu();
        WorkingPanel *GetWorkingPanel() { return customPanel.get();}
        void Deserialize(SerializableAppData *appdata);
        void ResetPatchUI();
        CREATE_FUNC(MainScene);
    };
}

#endif // __HELLOWORLD_SCENE_H__
