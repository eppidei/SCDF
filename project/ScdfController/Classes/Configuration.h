//
//  Configuration.h
//  ScdfController
//
//  Created by Andrea Scuderi on 25/01/15.
//
//

#ifndef __ScdfController__Configuration__
#define __ScdfController__Configuration__

#include <stdio.h>
#include <string>

namespace ScdfCtrl {
    class Configuration
    {
        static Configuration *_instance;
        Configuration();
    public:
        static Configuration *Instance()
        {
            if (NULL==_instance) _instance=new Configuration();
            return _instance;
        }
        
        bool oscEnabled;
        std::string port, ip;
        int midiDeviceIndex;
    };
}
#endif /* defined(__ScdfController__Configuration__) */