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

s_int32 UDPSendersManager::CreateSender(std::vector<s_int32> udpPorts, std::string ipAdd)
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

void UDPSendersManager::SetOutputPort(s_int32 port)
{
     LOGD("UDP Port Selected: %d \n", port);
}

void UDPSendersManager::SetOutputAddress(std::string ipAddress)
{
     LOGD("UPD IP Address selected %s \n", ipAddress.c_str());
}

void UDPSendersManager::SetMultiOutput(bool multiOutput)
{
    if(multiOutput)
    {
        LOGD("MULTI OUTPUT ON \n");
    } else
    {
        LOGD("MULTI OUTPUT OFF \n");
    }
}

void UDPSendersManager::SetOutputType(OutputSenderType type)
{
    if(type==OutputSenderType::UDP)
    {
        LOGD("OUTPUT TO UDP \n");
        
    } else if(type==OutputSenderType::OSC)
    {
        LOGD("OUTPUT TO OSC \n");
    }
}

s_int32 UDPSendersManager::GetOutputPort()
{
    return -1;
}

std::string UDPSendersManager::GetOutputAddress()
{
    return "";
}

bool UDPSendersManager::GetMultiOutput()
{
    return false;
}

OutputSenderType UDPSendersManager::GetOutputType()
{
    return (OutputSenderType)-1;
}
