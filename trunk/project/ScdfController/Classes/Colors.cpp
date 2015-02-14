//
//  Colors.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 14/02/15.
//
//

#include "Colors.h"

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
        Color3B(220,220,220),   //DropDownText
        Color3B::WHITE          //LabelText
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
