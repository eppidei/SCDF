//
//  UDPSendersManager.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 15/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "UDPSender.h"
#include "UDPSendersManager.h"

using namespace scdf;
UDPSendersManager *UDPSendersManager::_instance=NULL;

s_int32 UDPSendersManager::CreateSender(vector<s_int32> udpPorts, string ipAdd)
{
    if (udpPorts.size()==0) return -1;
    
    UDPSenderHelperBase *sender=new UDPSenderHelperBase();
    senders.push_back(sender);
    activeSender=senders.size()-1;
    sender->Init(udpPorts, ipAdd);
    return activeSender;
}

UDPSenderHelperBase *UDPSendersManager::GetActiveSender()
{
    assert(activeSender>-1 && activeSender<senders.size());
    if (activeSender<0 || activeSender>=senders.size()) return NULL;
    return senders[activeSender];
    
}