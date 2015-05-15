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
#define MAIN_BUTTON_NEW                     SCDFC_ID_BASE+1
#define MAIN_BUTTON_SAVE                    SCDFC_ID_BASE+2
#define MAIN_BUTTON_LOAD                    SCDFC_ID_BASE+3
#define MAIN_BUTTON_EDIT                    SCDFC_ID_BASE+4

#define MAIN_BUTTON_HIDESHOW_SCROLLVIEW     SCDFC_ID_BASE+5
#define MAIN_BUTTON_HIDESHOW_PROPERTIES     SCDFC_ID_BASE+6
#define DELETE_ITEM                         SCDFC_ID_BASE+7

//Working panel items IDs

#define MAX_NUM_ITEM 100
#define ITEMS_ID_BASE    SCDFC_ID_BASE+100
#define ITEM_SLIDER_ID   ITEMS_ID_BASE
#define ITEM_PAD_ID      ITEM_SLIDER_ID    + MAX_NUM_ITEM
#define ITEM_KNOB_ID     ITEM_PAD_ID       + MAX_NUM_ITEM
#define ITEM_SWITCH_ID   ITEM_KNOB_ID      + MAX_NUM_ITEM
#define ITEM_KEYBOARD_ID ITEM_SWITCH_ID    + MAX_NUM_ITEM
#define ITEM_MULTIPAD_ID ITEM_KEYBOARD_ID  + MAX_NUM_ITEM
#define ITEM_WHEEL_ID ITEM_MULTIPAD_ID  + MAX_NUM_ITEM

//Working panel items size base

#define SCROLLBAR_ITEM_SIDE_BASE    4
#define SLIDER_SIZE_BASE            cocos2d::Size(2,10)
#define WHEEL_SIZE_BASE             SLIDER_SIZE_BASE
#define PAD_SIZE_BASE               cocos2d::Size(5,5)
#define KEYBOARD_SIZE_BASE          cocos2d::Size(25,15)
#define MULTIPAD_SIZE_BASE          cocos2d::Size(12,8)
#define KNOB_SIZE_BASE              cocos2d::Size(8,8)
#define SWITCH_SIZE_BASE            cocos2d::Size(6,2)
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
#define PATCH_DELETE                     12
#define PROPERTIES_SUBPANELS_TOGGLE_HIDESHOW     13
#define PROPERTIES_CONTROLMODE_BASE    14
#define PROPERTIES_CONTROLMODE_WIRE     PROPERTIES_CONTROLMODE_BASE
#define PROPERTIES_CONTROLMODE_BLOW     15
#define PROPERTIES_CONTROLMODE_SNAP     16
#define PROPERTIES_CONTROLMODE_ROLL     17
#define PROPERTIES_CONTROLMODE_GESTURE  18
#define PROPERTIES_CONTROLMODE_LAST     PROPERTIES_CONTROLMODE_GESTURE
#define PROPERTIES_ITEMSETTINGS_MASTER  40
#define PROPERTIES_ITEMSETTINGS_GROUP 41
#define PROPERTIES_ITEMSETTINGS_COLOR 42
#define PROPERTIES_ITEMSETTINGS_ORIENT 43

#define PROPERTIES_MIDI_DEVICE 44
#define PROPERTIES_MIDI_MESSAGE 45
#define PROPERTIES_MIDI_CONTROL 46
#define PROPERTIES_MIDI_CHANNEL 47
#define PROPERTIES_MIDI_VELOCITY 48


#endif
