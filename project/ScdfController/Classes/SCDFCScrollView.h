//
//  SCDFCScollView.h
//  ScdfController
//
//  Created by Andrea Scuderi on 15/11/14.
//
//

#ifndef __ScdfController__SCDFCScollView__
#define __ScdfController__SCDFCScollView__

namespace ScdfCtrl {
    class MainScene;
    
    class ItemScrollView : public cocos2d::ui::ScrollView
    {
        MainScene *parent;
        template <class ItemType> void AddButtonToScrollView(std::string image);
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        void DragItemOnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::TouchEventType type);
        float RetrieveButtonYCoordInScrollview(cocos2d::ui::Button* button);
        template <class ItemType> void DoDragItemOnTouchEvent(cocos2d::ui::TouchEventType type, cocos2d::ui::Button* button);
    public:
        static ItemScrollView *CreateCustomScrollView(MainScene *main, cocos2d::Rect r);
        CREATE_FUNC(ItemScrollView);
    };
}
#endif /* defined(__ScdfController__SCDFCScollView__) */
