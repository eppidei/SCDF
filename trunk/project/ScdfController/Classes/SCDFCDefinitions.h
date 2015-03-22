//
//  SCDFCDefinitions.h
//  ScdfController
//
//  Created by Andrea Scuderi on 19/11/14.
//
//

#ifndef ScdfController_SCDFCDefinitions_h
#define ScdfController_SCDFCDefinitions_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#define SCDFC_ID_BASE 100
#define ID_TOOLBAR                          SCDFC_ID_BASE+1
#define TOOLBAR_BUTTON_GRID                 SCDFC_ID_BASE+2
#define TOOLBAR_BUTTON_ACTIVATE             SCDFC_ID_BASE+3
#define TOOLBAR_BUTTON_HIDESHOW_TOOLBAR     SCDFC_ID_BASE+4
#define TOOLBAR_BUTTON_HIDESHOW_SCROLLVIEW  SCDFC_ID_BASE+5
#define TOOLBAR_BUTTON_HIDESHOW_PROPERTIES  SCDFC_ID_BASE+6

#define MAX_NUM_ITEM 100
#define ITEMS_ID_BASE    SCDFC_ID_BASE+100
#define ITEM_SLIDER_ID   ITEMS_ID_BASE
#define ITEM_PAD_ID      ITEM_SLIDER_ID    + MAX_NUM_ITEM
#define ITEM_KNOB_ID     ITEM_PAD_ID       + MAX_NUM_ITEM
#define ITEM_SWITCH_ID   ITEM_KNOB_ID      + MAX_NUM_ITEM
#define ITEM_KEYBOARD_ID ITEM_SWITCH_ID    + MAX_NUM_ITEM
#define ITEM_MULTIPAD_ID ITEM_KEYBOARD_ID  + MAX_NUM_ITEM


#define SCROLLBAR_ITEM_SIDE_BASE    4
#define SLIDER_SIZE_BASE            cocos2d::Size(2,8)
#define SLIDER_HORZ_SIZE_BASE       cocos2d::Size(8,2)
#define PAD_SIZE_BASE               cocos2d::Size(4,4)
#define KEYBOARD_SIZE_BASE          cocos2d::Size(25,15)
#define MULTIPAD_SIZE_BASE          cocos2d::Size(12,8)
#define KNOB_SIZE_BASE              cocos2d::Size(8,8)
#define SQUARE_SENSOR_SIZE_BASE     PAD_SIZE_BASE_BASE
#define RECT_SENSOR_SIZE_BASE       SLIDER_SIZE_BASE

//Properties subpanel items ID

#define PROPERTIES_ITEM_NAME            1
#define PROPERTIES_ITEM_COLOR           2
#define PROPERTIES_ITEM_HEIGHT_MINUS    3
#define PROPERTIES_ITEM_HEIGHT_PLUS     4
#define PROPERTIES_ITEM_WIDTH_MINUS     5
#define PROPERTIES_ITEM_WIDTH_PLUS      6
#define PROPERTIES_OSC_TOGGLE           7
#define PROPERTIES_OSC_PORT             8
#define PROPERTIES_OSC_IP               9
#define PATCH_LOAD                      10
#define PATCH_SAVE                      11
#define PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW     12

#endif
