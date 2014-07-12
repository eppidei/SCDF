//
//  UDPSendersManager.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 15/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__UDPSendersManager__
#define __SCDF_Test__UDPSendersManager__

#include "TypeDefinitions.h"

namespace scdf {
    class UDPSenderHelperBase;

    class UDPSendersManager
    {
        static UDPSendersManager *_instance;
        UDPSendersManager() : multiOutput(true), oscPackData(true) {}
        s_int32 activeSender;
        std::vector<UDPSenderHelperBase*> senders;
        s_bool multiOutput, oscPackData;
        s_int32 CreateSender(std::vector<s_int32> udpPorts, std::string ipAdd);
    public:
        static UDPSendersManager *Instance()
        {
            if (NULL==_instance) _instance=new UDPSendersManager();
            return _instance;
        }
        UDPSenderHelperBase *GetActiveSender();
        s_int32 InitSender(s_int32 udpPortBase, std::string ipAdd);
        void ReleaseSender();
        void SetOutputPort(s_int32 port);
        void SetOutputAddress(std::string ipAddress);
        void SetMultiOutput(s_bool multiOutput);
        void SetUseOSCPackaging(s_bool pack);
        
        s_int32 GetOutputPort();
        std::string GetOutputAddress();
        s_bool GetMultiOutput();
        s_bool UseOSCPackaging();
    };
}

#endif /* defined(__SCDF_Test__UDPSendersManager__) */
