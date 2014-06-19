//
//  UDPSendersManager.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 15/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__UDPSendersManager__
#define __SCDF_Test__UDPSendersManager__

#include <iostream>
#include <string>
#include <vector>
#include "TypeDefinitions.h"

namespace scdf {
    class UDPSenderHelperBase;

    class UDPSendersManager
    {
        static UDPSendersManager *_instance;
        UDPSendersManager(){}
        s_int32 activeSender;
        std::vector<UDPSenderHelperBase*> senders;
    public:
        static UDPSendersManager *Instance()
        {
            if (NULL==_instance) _instance=new UDPSendersManager();
            return _instance;
        }
        s_int32 CreateSender(std::vector<s_int32> udpPorts, std::string ipAdd);
        UDPSenderHelperBase *GetActiveSender();
    };
}

#endif /* defined(__SCDF_Test__UDPSendersManager__) */
