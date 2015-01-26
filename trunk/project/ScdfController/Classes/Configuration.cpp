//
//  Configuration.cpp
//  ScdfController
//
//  Created by Andrea Scuderi on 25/01/15.
//
//

#include "Configuration.h"

using namespace ScdfCtrl;

Configuration *Configuration::_instance=NULL;

Configuration::Configuration()
{
    oscEnabled=false;
    port="5000";
    ip="127.0.0.1";
    midiDeviceIndex=0;
}