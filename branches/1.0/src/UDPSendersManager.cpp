//
//  UDPSendersManager.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 15/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "SaveSettings.h"
#include "UDPSender.h"
#include "UDPSendersManager.h"
#include "Harvester.h"


using namespace scdf;
UDPSendersManager *UDPSendersManager::_instance=NULL;

s_int32 UDPSendersManager::CreateSender(std::vector<s_int32> udpPorts, std::string ipAdd)
{
    if (udpPorts.size()==0) return -1;
    
    UDPSenderHelperBase *sender=new UDPSenderHelperBase();
    senders.push_back(sender);
    activeSender=(s_int32)senders.size()-1;
    return sender->Init(udpPorts, ipAdd);
}

s_int32 UDPSendersManager::InitSender(s_int32 udpPortBase, std::string ipAdd)
{
    StopRestartMachine kk;
//    Harvester::Instance()->Stop();
    ReleaseSender();
    std::vector<int> udpPorts;
    udpPorts.push_back(udpPortBase);

    if (multiOutput)
        for (int i=1;i<scdf::NumTypes;++i)
            udpPorts.push_back(udpPortBase+i);
    
    return CreateSender(udpPorts, ipAdd);
}

void UDPSendersManager::ReleaseSender()
{
    for (int i=0;i<senders.size();++i)
        delete senders[i];
    senders.clear();
}

UDPSenderHelperBase *UDPSendersManager::GetActiveSender()
{
    if (activeSender<0 || activeSender>=senders.size()) return NULL;
    return senders[activeSender];
    
}

void UDPSendersManager::SetOutputPort(s_int32 port)
{
    scdf::SaveSettings::Instance()->SaveIntValue(port,PORT_IP_KEY);
    
    if (0==senders.size()) return;
    InitSender(port, GetOutputAddress());
   
}

void UDPSendersManager::SetOutputAddress(std::string ipAddress)
{
    scdf::SaveSettings::Instance()->SaveStringValue(ipAddress,ADDRESS_IP_KEY);
    
    if (0==senders.size()) return;
    InitSender(GetOutputPort(), ipAddress);
}

void UDPSendersManager::SetMultiOutput(s_bool _multiOutput)
{
    scdf::SaveSettings::Instance()->SaveBoolValue(_multiOutput, MULTI_ROUTE_KEY);
    
    multiOutput=_multiOutput;
    if (0==senders.size()) return;
    InitSender(GetOutputPort(), GetOutputAddress());
    
}

void UDPSendersManager::SetUseOSCPackaging(s_bool pack)
{
    scdf::SaveSettings::Instance()->SaveBoolValue(pack, OUTPUT_TYPE);
    oscPackData=pack;
}

s_int32 UDPSendersManager::GetOutputPort()
{
    if (0==senders.size()) return -1;
    return senders[0]->GetPort();
}

std::string UDPSendersManager::GetOutputAddress()
{
    if (0==senders.size()) return "No Sender";
    return senders[0]->GetAddress();
}

s_bool UDPSendersManager::GetMultiOutput()
{
    return multiOutput;
}

s_bool UDPSendersManager::UseOSCPackaging()
{
    return oscPackData;
}