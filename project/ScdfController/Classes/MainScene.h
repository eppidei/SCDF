#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

namespace cocos2d{
    class Layer;
}
namespace SCDFC{
    class WorkingPanel;
    class SScrollView;

    class MainScene : public cocos2d::Layer
    {
        std::vector<float> gridUnity;
        std::unique_ptr<SScrollView> customScrollView;
        std::unique_ptr<WorkingPanel> customPanel;
        cocos2d::ui::ImageView *draggingImage;
        void AddToolbar(cocos2d::Rect r);
        void CalculateGrid();
        int gridIndex;
        void OnGridButtonClick();
        void HideShowToolbar();
        void HideShowScrollview();
    public:
        // there's no 'id' in cpp, so we recommend returning the class instance pointer
        static cocos2d::Scene* createScene();

        // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
        virtual bool init();  
        
        void touchEvent(Ref *pSender, cocos2d::ui::TouchEventType type);
        template <class ItemType> void  OnStartDragging(cocos2d::Vec2 dragStartPoint);
        void OnDragging(cocos2d::Rect draggingRect);
        template <class ItemType> void  OnEndDragging();
        int GetGridBase();
        
        int GetGridDistance();
        void SnapToGrid(cocos2d::Rect &r);
        void EnableScrollView(bool enable);
        // implement the "static create()" method manually
        CREATE_FUNC(MainScene);
    };
}

#endif // __HELLOWORLD_SCENE_H__
