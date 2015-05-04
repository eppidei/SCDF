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
        void InitItemsColors();
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
            UIColorsId_Count
        };
        enum ItemsColorsId
        {
            White,
            Yellow,
            Blue,
            Green,
            Red,
            Magenta,
            Black,
            Orange,
            Gray,
            ItemsColorsId_Count
        };
        enum FontsId
        {
            PropHeader,
            DropDownMenuLabel,
            DropDownMenu,
            ItemLabel,
            LoadSaveElement,
            FontsId_Count
        };
        static Colors *Instance();
        cocos2d::Color3B GetUIColor(UIColorsId which);
        cocos2d::Color3B GetItemsColor(ItemsColorsId which);
        std::string GetFontPath(FontsId fID);
        int GetFontSize(FontsId fID);
        int CountItemsColor();
    };
}
#endif /* defined(__ScdfController__Colors__) */
