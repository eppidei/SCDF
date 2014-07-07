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

enum OutputSenderType{ UDP, OSC};

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
        
        void SetOutputPort(s_int32 port);
        void SetOutputAddress(std::string ipAddress);
        void SetMultiOutput(bool multiOutput);
        void SetOutputType(OutputSenderType type);
        
        s_int32 GetOutputPort();
        std::string GetOutputAddress();
        bool GetMultiOutput();
        OutputSenderType GetOutputType();
    };
}

#endif /* defined(__SCDF_Test__UDPSendersManager__) */
