//
//  UDPSender.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "UdpSocket.h"
#include "UDPSender.h"
#include "UDPSendersManager.h"
#include "osc/OscOutboundPacketStream.h"

using namespace scdf;

void UDPSender::Init(int udpp, std::string add)
{
	endPoint.reset(new IpEndpointName(IpEndpointName(add.c_str(), udpp)));
	transmitSocket.reset(new UdpTransmitSocket((*(endPoint.get()))));
}

void UDPSender::Release()
{
    transmitSocket.reset();
	endPoint.reset();
}

void UDPSender::SendData(s_char* data, s_int32 size)
{
/*    s_char buffer[4096];
	osc::OutboundPacketStream oscPack( buffer, 4096);
	oscPack << osc::BeginBundleImmediate
	<< osc::BeginMessage( "/Sensor type") << ((SensorData*)data)->type << osc::EndMessage
    << osc::BeginMessage( "/Rate"	) << ((SensorData*)data)->rate << osc::EndMessage
    << osc::BeginMessage( "/Bit"	) << ((SensorData*)data)->sample_bit_resolution << osc::EndMessage
    << osc::BeginMessage( "/Buffer size"	) << ((SensorData*)data)->num_samples << osc::EndMessage
    << osc::BeginMessage( "/Timestamp"	) << ((SensorData*)data)->timestamp << osc::EndMessage;
	oscPack << osc::EndBundle;
	
    transmitSocket->Send(oscPack.Data(), oscPack.Size());*/
	transmitSocket->Send(data, size);
}

void UDPSenderHelperBase::SendData()
{
    switch (senders.size()) {
        case 0:     return;
        case 1:     DoSendData(); break;
        default:    DoMultiSendData(); break;
    }
}

void UDPSenderHelperBase::DoSendData()
{
#define BIT_PER_BYTE 8
    for (int i=0;i<senderData.size();++i)
    {
        s_int32 size=(senderData[i]->num_samples*sizeof(s_sample))/BIT_PER_BYTE;
        senders[0]->SendData((s_char*)senderData[i]->data, size);
    }
}

void UDPSenderHelperBase::DoMultiSendData()
{
    //send data according to senders number
}

static void SentDataRecyclingProcedure(std::vector <SensorData*> *data)
{
    for (int i=0;i<data->size();++i)
        delete (*data)[i];
    data->clear();
    UDPSendersManager::Instance()->GetActiveSender()->EventFreeSlot()->Set();
    
}

void UDPSenderHelperBase::SendOnThread()
{
    EventCanSend()->Wait();
    SendData();
    SentDataRecyclingProcedure(&senderData);
}

static void UDPSenderHelperProcedure(void *param)
{
    UDPSenderHelperBase *sender=((UDPSenderHelperBase*)param);
    while(sender->activated)
    {
        sender->SendOnThread();
    }
    delete sender;
}

UDPSenderHelperBase::UDPSenderHelperBase() : activated(true), freeSlot(1), canSend(0)
{
}

void UDPSenderHelperBase::Init(std::vector<s_int32> udpPorts, std::string address)
{
    //_ASSERT(udpPorts.size()!=0);
    for (int i=0;i<udpPorts.size();++i)
        senders.push_back(new UDPSender(udpPorts[i],address));
    ThreadUtils::CreateThread((start_routine)UDPSenderHelperProcedure, this);
}
