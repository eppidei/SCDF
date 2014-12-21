//
//  SCDFCWorkingPanel.h
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#ifndef __ScdfController__SCDFCWorkingPanel__
#define __ScdfController__SCDFCWorkingPanel__

namespace cocos2d{
    namespace ui{
        class ListView;
    }
}
namespace SCDFC{
    class MainScene;
    class ItemBase;
    
    class WorkingPanel : public cocos2d::ui::Layout
    {
        bool drawGrid = false;
        MainScene *parent;
        cocos2d::Rect draggingRect;
        std::vector <ItemBase*> items;
        bool active;
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void DrawGrid();
        void OnControlTouch(Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void CheckRemoveControl(Node *n);
    public:
        bool OnControlMove(Ref *pSender, cocos2d::Vec2 touchPos, cocos2d::ui::Widget::TouchEventType type);
        void CheckAddControl(int buttonTag);
        void SetDraggingRect(cocos2d::Rect _draggingRect);
        virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);
        static WorkingPanel *CreateCustomPanel(MainScene *main, cocos2d::Rect r);
        void selectedItemEvent(Ref *pSender, cocos2d::ui::ListView::EventType type);
        void ToggleActiveState();
        CREATE_FUNC(WorkingPanel);
    };
}
#endif /* defined(__ScdfController__SCDFCWorkingPanel__) */
