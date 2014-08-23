//
//  SaveSettings.h
//  SCDF_Test
//
//  Created by Marco Bertola on 16/07/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//
#pragma once

#include "Sensor.h"

#define ADDRESS_IP_KEY      "ip_address"
#define PORT_IP_KEY         "ip_port"
#define MULTI_ROUTE_KEY     "multi_route"
#define OUTPUT_TYPE         "outuput_type"

namespace scdf {
    
    
    class SaveSettings
    {
        
    public:
        void  SaveIntValue(s_int32 _value, std::string valueKey) ;
        void  SaveStringValue(std::string _value, std::string valueKey);
        void  SaveBoolValue(s_bool _value, std::string valueKey);
        
        s_int32        LoadIntValue(std::string valueKey);
        std::string*   LoadStringValue(std::string valueKey);
        s_bool         LoadBoolValue(std::string valueKey);
        
        static SaveSettings *_instance;
        static SaveSettings *Instance()
        {
            if (NULL==_instance) _instance=new SaveSettings();
            return _instance;
        }
        
    };

    
}

