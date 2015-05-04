//
//  Colors.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 14/02/15.
//
//

#include "Colors.h"
#include "OsUtilities.h"

using namespace ScdfCtrl;
using namespace cocos2d;

Colors *Colors::_instance=NULL;

Colors *Colors::Instance()
{
    if (NULL==_instance)
        _instance= new Colors();
    return _instance;
}

cocos2d::Color3B Colors::GetUIColor(UIColorsId which)
{
    return theUIColors[which];
}

cocos2d::Color3B Colors::GetItemsColor(ItemsColorsId which)
{
    return theItemsColors[which];
}

int Colors::CountItemsColor()
{
    return ItemsColorsId_Count;
}

void Colors::InitUIColors()
{
    cocos2d::Color3B scdfc_ui_Colors[]={
        Color3B(50,50,50),      //Main_Background
        Color3B(1,80,80),       //SubPanel
        Color3B::WHITE,         //DropDownText
        Color3B::WHITE,         //LabelText
        Color3B(165,204,47),    //LabelTextOn
        Color3B(109,109,109),   //LabelBackGroundGradientTop
        Color3B(48,48,48),      //LabelBackGroundGradientBottom
        Color3B(171,171,171),   //LabelHeaderBackGround
        Color3B(165,204,47),    //WidgetBackGround
        Color3B::WHITE,         //SubpanelGenericItem
    };
    
    for(int i=0; i<UIColorsId_Count; ++i)
        theUIColors.push_back(scdfc_ui_Colors[i]);
}

void Colors::InitItemsColors()
{
    cocos2d::Color3B scdfc_items_Colors[]={
        Color3B::WHITE,
        Color3B::YELLOW,
        Color3B::BLUE,
        Color3B::GREEN,
        Color3B::RED,
        Color3B::MAGENTA,
        Color3B::BLACK,
        Color3B::ORANGE,
        Color3B::GRAY
    };
    
    for(int i=0; i<ItemsColorsId_Count; ++i)
        theItemsColors.push_back(scdfc_items_Colors[i]);
}

Colors::Colors()
{
    InitItemsColors();
    InitUIColors();
}

int Colors::GetFontSize(FontsId fID)
{
    int size=10;
    switch (fID)
    {
        case PropHeader:
            size=22;
            break;
        case DropDownMenuLabel:
            size=20;
            break;
        case ItemLabel:
            size=15;
            break;
        case DropDownMenu:
            size=20;
            break;
        case LoadSaveElement:
            size=22;
        default:
            break;
    }
    return size;
}

std::string Colors::GetFontPath(FontsId fID)
{
    std::string filename="";
    switch (fID)
    {
        case PropHeader:
        case DropDownMenuLabel:
        case ItemLabel:
        case LoadSaveElement:
            filename="OpenSans-CondLight";
            break;
        case DropDownMenu:
            filename="Ds_Digi";
            break;
        default:
            break;
    }
    if (!filename.length()) return filename;
    return scdf::GetDirectoryForFile(filename, "ttf");
}
