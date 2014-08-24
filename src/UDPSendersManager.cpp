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

UDPSendersManager::UDPSendersManager() : senderHelper(new UDPSenderHelperBase())
{
    InitSender(DEFAULT_UDP_PORT_BASE, DEFAULT_IP_ADDRESS);
}

void UDPSendersManager::InitSender(s_int32 udpPortBase, std::string ipAdd)
{
    senderHelper->Init(udpPortBase, ipAdd);
}

UDPSenderHelperBase *UDPSendersManager::GetSender()
{
    return senderHelper.get();
}

void UDPSendersManager::SetOutputPort(s_int32 port)
{
    scdf::SaveSettings::Instance()->SaveIntValue(port,PORT_IP_KEY);
    InitSender(port, GetOutputAddress());
}

void UDPSendersManager::SetOutputAddress(std::string ipAddress)
{
    scdf::SaveSettings::Instance()->SaveStringValue(ipAddress,ADDRESS_IP_KEY);
    InitSender(GetOutputPort(), ipAddress);
}

void UDPSendersManager::SetMultiOutput(s_bool _multiOutput)
{
    scdf::SaveSettings::Instance()->SaveBoolValue(_multiOutput, MULTI_ROUTE_KEY);
    senderHelper->multiOutput=_multiOutput;
}

void UDPSendersManager::SetUseOSCPackaging(s_bool pack)
{
    scdf::SaveSettings::Instance()->SaveBoolValue(pack, OUTPUT_TYPE);
    senderHelper->oscPackData=pack;
}

s_int32 UDPSendersManager::GetOutputPort()
{
    return senderHelper->GetPort();
}

std::string UDPSendersManager::GetOutputAddress()
{
    return senderHelper->GetAddress();
}

s_bool UDPSendersManager::GetMultiOutput()
{
    return senderHelper->multiOutput;
}

s_bool UDPSendersManager::UseOSCPackaging()
{
    return senderHelper->oscPackData;
}
