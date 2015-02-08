#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "SCDFCDefinitions.h"

namespace ScdfCtrl
{
    class WorkingPanel;
    class ItemScrollView;
    class PropertiesPanel;
    class ItemBase;
    class LoadSavePanel;

    class MainScene : public cocos2d::Layer
    {
        std::vector<float> gridUnity;
        std::unique_ptr<ItemScrollView> customScrollView;
        std::unique_ptr<WorkingPanel> customPanel;
        std::unique_ptr<PropertiesPanel> propertiesPanel;
        std::unique_ptr<LoadSavePanel> loadSavePanel;
        
        cocos2d::ui::ImageView *draggingImage;
        void AddToolbar(cocos2d::Rect r);
        void CalculateGrid();
        int gridIndex;
        void OnGridButtonClick();
        void HideShowToolbar();
        void HideShowScrollview();
        void HideShowPropertiesPanel();
        void HideShowLoadSavePanel();
    public:
        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();  
        
        void touchEvent(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        //void touchEvent(Ref *pSender, cocos2d::ui::TouchEventType type);
        template <class ItemType> void  OnStartDragging(cocos2d::Vec2 dragStartPoint);
        void OnDragging(cocos2d::Rect draggingRect);
        template <class ItemType> void  OnEndDragging();
        static int GetUnityBase();
        
        int GetGridDistance();
        void SnapToGrid(cocos2d::Rect &r);
        void EnableScrollView(bool enable);
        void AttachItem(ItemBase *item);
        void DetachItem(ItemBase *item);
        // implement the "static create()" method manually
        CREATE_FUNC(MainScene);
    };
}

#endif // __HELLOWORLD_SCENE_H__
