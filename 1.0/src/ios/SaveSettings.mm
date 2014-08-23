//
//  SaveSettings.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 16/07/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//


#include "SaveSettings.h"

using namespace scdf;

SaveSettings *SaveSettings::_instance=NULL;


void SaveSettings::SaveIntValue(s_int32 _value, std::string valueKey)
{
     NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
     NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    [prefs setInteger:_value forKey:key];
}
void SaveSettings::SaveStringValue(std::string _value, std::string valueKey)
{
     NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
     NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    [prefs setObject:[NSString stringWithUTF8String:_value.c_str()] forKey:key];
}

void  SaveSettings::SaveBoolValue(s_bool _value, std::string valueKey)
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    [prefs setBool:_value forKey:key];
}



//////////// ************* LOAD ****************** ////////////////

s_int32  SaveSettings::LoadIntValue(std::string valueKey)
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    return (s_int32)[prefs integerForKey:key];
}

std::string* SaveSettings::LoadStringValue(std::string valueKey)
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    NSString *value = [prefs stringForKey:key];
    if(!value)
        return NULL;
    return new std::string([value UTF8String]);
}

s_bool SaveSettings::LoadBoolValue(std::string valueKey)
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSString *key = [NSString stringWithUTF8String:valueKey.c_str()];
    return (s_bool)[prefs boolForKey:key];
}
