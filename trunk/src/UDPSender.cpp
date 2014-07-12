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
#include "CustomPipe.h"
#include "PipesManager.h"
#include "Harvester.h"

using namespace scdf;

s_int32 UDPSender::Init(int udpp, std::string add)
{
    address=add;
    s_int32 res=1;
    try
    {
        endPoint.reset(new IpEndpointName(IpEndpointName(add.c_str(), udpp)));
        transmitSocket.reset(new UdpTransmitSocket((*(endPoint.get()))));
    } catch (...)
    {
        assert(false);
        res=0;
    }
	return res;
}

s_int32 UDPSender::GetPort()
{
    if (NULL==endPoint.get()) return -1;
    return endPoint->port;
}

std::string UDPSender::GetAddress()
{
    return address;
}

void UDPSender::Release()
{
    address="Not Assigned";
    transmitSocket.reset();
	endPoint.reset();
}

void UDPSender::SendData(s_char* data, s_int32 size)
{
#ifdef LOG_UDP_SEND
	LOGD("UDPSender send data\n");
#endif
	transmitSocket->Send(data, size);
}

void UDPSender::SendDataOSCPacked(osc::OutboundPacketStream &oscData)
{
#ifdef LOG_UDP_SEND
	LOGD("UDP send data osc packed\n");
#endif
    transmitSocket->Send(oscData.Data(), oscData.Size());
}

void UDPSenderHelperBase::SendData()
{
    switch (senders.size()) {
        case 0:
            return;
        case 1:
            if (UDPSendersManager::Instance()->UseOSCPackaging())
                DoSendDataOSCPacked();
            else
                DoSendData();
            break;
        default:
            if (UDPSendersManager::Instance()->UseOSCPackaging())
                DoMultiSendDataOSCPacked();
            else
                DoMultiSendData();
            break;
    }
}

void UDPSenderHelperBase::DoSendData()
{
    for (int i=0;i<senderData.size();++i)
    {
        senders[0]->SendData((s_char*)senderData[i], sizeof(SensorData));
        senders[0]->SendData((s_char*)senderData[i]->data, senderData[i]->num_samples*sizeof(s_sample));
    }
}

void UDPSenderHelperBase::DoMultiSendData()
{
#ifdef LOG_SENDER_DATA
    LOGD("MultiSendData, Master sensor: %s\n", SensorTypeString[Harvester::Instance()->GetType()].c_str());
#endif
    for (int i=0;i<senderData.size();++i)
    {
        s_int32 size=sizeof(SensorData)-sizeof(char*)+senderData[i]->num_samples*sizeof(s_sample);
        s_char *temp=(char*)malloc(size);
        memcpy(temp, senderData[i], sizeof(SensorData));
        memcpy(temp+sizeof(SensorData)-sizeof(char*),senderData[i]->data, senderData[i]->num_samples*sizeof(s_sample));
        //int pluto2=sizeof(senderData[i]->type);
        
       // senders[senderData[i]->type]->SendData((s_char*)senderData[i], sizeof(SensorData));
        //senders[senderData[i]->type]->SendData((s_char*)senderData[i]->data, senderData[i]->num_samples*sizeof(s_sample));
        senders[senderData[i]->type]->SendData(temp,size);
        free(temp);
    }
}

void UDPSenderHelperBase::DoSendDataOSCPacked()
{
    for (int i=0;i<senderData.size();++i)
    {
        s_char buffer[OSC_BUFFER_SIZE];
        osc::OutboundPacketStream oscData( buffer, OSC_BUFFER_SIZE);
        OSCPackData(senderData[i], oscData);
        senders[0]->SendDataOSCPacked(oscData);
    }
}

void UDPSenderHelperBase::DoMultiSendDataOSCPacked()
{
    for (int i=0;i<senderData.size();++i)
    {
        s_char buffer[OSC_BUFFER_SIZE];
        osc::OutboundPacketStream oscData( buffer, OSC_BUFFER_SIZE);
        OSCPackData(senderData[i], oscData);
        senders[senderData[i]->type]->SendDataOSCPacked(oscData);
    }
}

void UDPSenderHelperBase::SendOnThread()
{
    EventCanSend()->Wait();
    if (!activated) return;
    SendData();
    Harvester::SentDataRecyclingProcedure(&senderData);
}

static void UDPSenderHelperProcedure(void *param)
{
    UDPSenderHelperBase *sender=((UDPSenderHelperBase*)param);
    while(sender->activated)
    {
        sender->SendOnThread();
    }
}

UDPSenderHelperBase::UDPSenderHelperBase() : activated(false), freeSlot(1), canSend(0)
{
}

s_int32 UDPSenderHelperBase::GetPort()
{
    if (senders.size()==0) return -1;
    return senders[0]->GetPort();
}

std::string UDPSenderHelperBase::GetAddress()
{
    if (senders.size()==0) return "No Sender";
    return senders[0]->GetAddress();
}

void UDPSenderHelperBase::Init(std::vector<s_int32> udpPorts, std::string address)
{
    assert(udpPorts.size()!=0);
    for (int i=0;i<udpPorts.size();++i)
    {
        UDPSender *s=new UDPSender();
        if (1==s->Init(udpPorts[i],address))
            senders.push_back(s);
    }
    if (0==senders.size())
        return;
    activated=true;
    handle=ThreadUtils::CreateThread((start_routine)UDPSenderHelperProcedure, this);
}

void UDPSenderHelperBase::Release()
{
    activated=false;
    EventCanSend()->Set();
    EventFreeSlot()->Set();
    ThreadUtils::JoinThread(handle);
    for (int i=0;i<senders.size();++i)
        delete senders[i];
    senders.clear();
}

void UDPSenderHelperBase::OSCPackData(SensorData *data, osc::OutboundPacketStream &oscData)
{
#ifdef LOG_SENDER_DATA
   // for (int i = 0; i< ((SensorData*)data)->num_samples; i ++) {
    printf("Sending data from %s, Master sensor: %s\n",SensorTypeString[((SensorData*)data)->type].c_str(), SensorTypeString[Harvester::Instance()->GetType()].c_str());
   // }
#endif
    switch(data->type)
    {
       /* case AudioInput:
        {
            SensorAudioData *audioData=(SensorAudioData*)data;
            oscData << osc::BeginBundle()
            << osc::BeginMessage( "/Sensor type") << audioData->type << osc::EndMessage
            << osc::BeginMessage( "/Rate"	) << audioData->rate << osc::EndMessage
            << osc::BeginMessage( "/Channels"	) << audioData->numChannels << osc::EndMessage
            << osc::BeginMessage( "/Num samples") << audioData->num_samples << osc::EndMessage
            << osc::BeginMessage( "/Time ref") << (osc::int64)audioData->timeid << osc::EndMessage
            << osc::BeginMessage( "/Timestamp") << (osc::int64)audioData->timestamp << osc::EndMessage
            << osc::BeginMessage( "/Data:"	) << osc::Blob(data->data, data->num_samples*sizeof(s_sample))<< osc::EndMessage;
            oscData << osc::EndBundle;
        }
            break;*/
        default:
            oscData << osc::BeginBundle()
            << osc::BeginMessage( "/Sensor type") << data->type << osc::EndMessage
            << osc::BeginMessage( "/Rate"	) << data->rate << osc::EndMessage
            << osc::BeginMessage( "/Channels"	) << data->numChannels << osc::EndMessage
            << osc::BeginMessage( "/Num samples") << data->num_samples << osc::EndMessage
            << osc::BeginMessage( "/Time ref") << (osc::int64)data->timeid << osc::EndMessage
            << osc::BeginMessage( "/Timestamp") << (osc::int64)data->timestamp << osc::EndMessage
            << osc::BeginMessage( "/Data:"	) << osc::Blob(data->data, data->num_samples*sizeof(s_sample))<< osc::EndMessage;
            oscData << osc::EndBundle;
            break;
    }
}
