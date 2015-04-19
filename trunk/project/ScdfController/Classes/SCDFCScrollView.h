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
    
    class ItemScrollView : public cocos2d::ui::Layout
    {
        std::map<int, ItemScrollView*> itemSubpanels;
        void InitWithContent(MainScene *main, cocos2d::Rect r);
        virtual void SubscribeButton(cocos2d::ui::Button *button, bool drag);
        template <class ItemType> void DoDragItemOnTouchEvent(cocos2d::ui::Widget::TouchEventType type, cocos2d::ui::Button* button);
        
        void CreateSubpanel(int itemID, std::vector<std::string> buttonsBtm, cocos2d::Rect r);
        void ToggleShowSubpanel(int itemID, float y);
        void CheckButton(int buttonID, bool check);
        
        virtual int GetBackgroundBitmapLeftOffset(){return 35;}
        virtual int GetBackgroundBitmapTopOffset(){return 30;}
        virtual int GetBackgroundBitmapBottomOffset(){return 80;}
        
        bool isOpened;
    protected:
        MainScene *parent;
        void DoInit(MainScene *main, cocos2d::Rect r);
        cocos2d::ui::ScrollView *scrollView;
        template <class ItemType> void AddButtonToScrollView();
        float RetrieveButtonYCoordInScrollview(cocos2d::ui::Button* button);
        void DragItemOnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
        void ToggleItemMenuOnTouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
    public:
        static ItemScrollView *CreateCustomScrollView(MainScene *main, cocos2d::Rect r);
        void SetDirection(cocos2d::ui::ScrollView::Direction dir);
        void HideAllSubPanels();
        void HideShow(bool hide);
        bool IsOpened() {return isOpened;}
        CREATE_FUNC(ItemScrollView);
    };
    class ItemScrollViewSubpanel : public ItemScrollView
    {
        void InitWithContentAndBitmaps(MainScene *main, cocos2d::Rect r, std::vector<std::string> buttonsBtm, int ownerID);
        void SubscribeButton(cocos2d::ui::Button *button, bool drag) override;
        int GetBackgroundBitmapLeftOffset() override {return 0;}
        int GetBackgroundBitmapTopOffset() override {return 0;}
        int GetBackgroundBitmapBottomOffset() override {return 0;}
    public:
        static ItemScrollView *CreateScrollViewSubPanel(MainScene *main, std::vector<std::string> buttonsBtm, cocos2d::Rect r, int ownerID);
        CREATE_FUNC(ItemScrollViewSubpanel);
    };
}
#endif /* defined(__ScdfController__SCDFCScollView__) */
