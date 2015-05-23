//
//  Colors.h
//  ScdfController
//
//  Created by Andrea Scuderi on 14/02/15.
//
//

#ifndef __ScdfController__Colors__
#define __ScdfController__Colors__

#include <stdio.h>
#include "SCDFCDefinitions.h"

namespace ScdfCtrl {
    
    class Colors
    {
        Colors();
        static Colors *_instance;
        std::vector<cocos2d::Color3B> theUIColors;
        std::vector<cocos2d::Color3B> theItemsColors;
        void InitUIColors();
//        void InitItemsColors();
    public:
        enum UIColorsId
        {
            Main_Background,
            SubPanel,
            DropDownText,
            LabelText,
            LabelTextOn,
            LabelBackGroundGradientTop,
            LabelBackGroundGradientBottom,
            LabelHeaderBackGround,
            WidgetBackGround,
            SubpanelGenericItem,
            ModalPanelText,
            UIColorsId_Count
        };
        enum FontsId
        {
            PropHeader,
            DropDownMenuLabel,
            DropDownMenu,
            ItemLabel,
            LoadSaveElement,
            PopupPanelText,
            FontsId_Count
        };
        enum ItemsColorsId
        {
            Violet,
            Pink,
            DarkRed,
            LightRed,
            Orange,
            Yellow,
            GreenUI,
            LightBlue,
            LightGrey,
            DarkGrey,
            ItemsColorsId_Count
        };
        void InitItemsColors()
        {
            cocos2d::Color3B scdfc_items_Colors[]={
                cocos2d::Color3B(191,53,255),
                cocos2d::Color3B(243,80,172),
                cocos2d::Color3B(231,0,76),
                cocos2d::Color3B(240,78,48),
                cocos2d::Color3B(253,185,19),
                cocos2d::Color3B(233,252,76),
                cocos2d::Color3B(166,206,57),
                cocos2d::Color3B(0,170,211),
                cocos2d::Color3B(168,168,168),
                cocos2d::Color3B(77,77,77)
            };
            
            for(int i=0; i<ItemsColorsId_Count; ++i)
                theItemsColors.push_back(scdfc_items_Colors[i]);
        }
        static Colors *Instance();
        cocos2d::Color3B GetUIColor(UIColorsId which);
        cocos2d::Color3B GetItemsColor(ItemsColorsId which);
        std::string GetFontPath(FontsId fID);
        int GetFontSize(FontsId fID);
        int CountItemsColor();
    };
}
#endif /* defined(__ScdfController__Colors__) */
