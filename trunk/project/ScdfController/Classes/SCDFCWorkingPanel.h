//
//  SCDFCWorkingPanel.h
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#ifndef __ScdfController__SCDFCWorkingPanel__
#define __ScdfController__SCDFCWorkingPanel__
#include "ControlUnit.h"
#include "SCDFCItems.h"

namespace ScdfCtrl
{
    class MainScene;
    
    class WorkingPanel : public cocos2d::ui::Layout
    {
        MainScene *parent;
        cocos2d::Rect draggingRect;
        bool collisionDetected, active;
        std::vector <ItemBase*> items;
        
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void DrawGrid();
        void CheckRemoveControl(Node *n);
        void DetectCollisions(Node *_item);
        void DoDetectCollisions(Node *_item, cocos2d::Rect r);
        
    public:
        bool OnControlMove(Ref *pSender, cocos2d::Vec2 touchPos, cocos2d::ui::Widget::TouchEventType type);
        void CheckAddControl(int buttonTag);
        void SetDraggingRect(cocos2d::Rect _draggingRect);
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags);
        static WorkingPanel *CreateCustomPanel(MainScene *main, cocos2d::Rect r);
        void ToggleActiveState();
        void DetectCollisions(cocos2d::Rect r);
        CREATE_FUNC(WorkingPanel);
    };
}
#endif /* defined(__ScdfController__SCDFCWorkingPanel__) */
