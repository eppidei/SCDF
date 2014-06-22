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
#include "Logging.h"

using namespace scdf;

void UDPSender::Init(int udpp, string add)
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
	LOGD("UDPSender send data");
	transmitSocket->Send(data, size);
}

void UDPSender::SendDataOSCPacked(osc::OutboundPacketStream &oscData)
{
	LOGD("UDP send data osc packed");
    transmitSocket->Send(oscData.Data(), oscData.Size());
}

void UDPSenderHelperBase::SendData()
{
    switch (senders.size()) {
        case 0:     return;
        case 1:     DoSendData(); /*DoSendDataOSCPacked();*/break;
        default:    /*DoMultiSendData();*/ DoMultiSendDataOSCPacked(); break;
    }
}

void UDPSenderHelperBase::DoSendData()
{
    for (int i=0;i<senderData.size();++i)
    {
        s_int32 size=sizeof(SensorData)-sizeof(char*)+senderData[i]->num_samples*sizeof(s_sample);
        senders[0]->SendData((s_char*)senderData[i], size);
    }
}

void UDPSenderHelperBase::DoMultiSendData()
{
    for (int i=0;i<senderData.size();++i)
    {
        s_int32 size=sizeof(SensorData)-sizeof(char*)+senderData[i]->num_samples*sizeof(s_sample);
        senders[senderData[i]->type]->SendData((s_char*)senderData[i], size);
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

static void SentDataRecyclingProcedure(vector <SensorData*> *data)
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

void UDPSenderHelperBase::Init(vector<s_int32> udpPorts, string address)
{
    assert(udpPorts.size()!=0);
    for (int i=0;i<udpPorts.size();++i)
        senders.push_back(new UDPSender(udpPorts[i],address));
    ThreadUtils::CreateThread((start_routine)UDPSenderHelperProcedure, this);
}

void UDPSenderHelperBase::OSCPackData(SensorData *data, osc::OutboundPacketStream &oscData)
{
#ifdef TEST_PRINT_DATA
    for (int i = 0; i< ((SensorData*)data)->num_samples; i ++) {
        printf("Sending data %d from %s: %.4f\n",i,SensorTypeString[((SensorData*)data)->type].c_str(), ((s_sample*)((SensorData*)data)->data)[i]);
    }
#endif
    switch(data->type)
    {
        case AudioInput:
        {
            SensorAudioData *audioData=(SensorAudioData*)data;
            oscData << osc::BeginBundle()
            << osc::BeginMessage( "/Sensor type") << audioData->type << osc::EndMessage
            << osc::BeginMessage( "/Rate"	) << audioData->rate << osc::EndMessage
            << osc::BeginMessage( "/Channels"	) << audioData->numChannels << osc::EndMessage
            << osc::BeginMessage( "/Num samples") << audioData->num_samples << osc::EndMessage
            << osc::BeginMessage( "/Time ref") << audioData->timeid << osc::EndMessage
            << osc::BeginMessage( "/Timestamp") << audioData->timestamp << osc::EndMessage
            << osc::BeginMessage( "/Data:"	) << osc::Blob(data->data, data->num_samples*sizeof(s_sample))<< osc::EndMessage;
            oscData << osc::EndBundle;
        }
            break;
        default:
            oscData << osc::BeginBundle()
            << osc::BeginMessage( "/Sensor type") << data->type << osc::EndMessage
            << osc::BeginMessage( "/Rate"	) << data->rate << osc::EndMessage
            << osc::BeginMessage( "/Num samples") << data->num_samples << osc::EndMessage
            << osc::BeginMessage( "/Time ref") << data->timeid << osc::EndMessage
            << osc::BeginMessage( "/Timestamp") << data->timestamp << osc::EndMessage
            << osc::BeginMessage( "/Data:"	) << osc::Blob(data->data, data->num_samples*sizeof(s_sample))<< osc::EndMessage;
            oscData << osc::EndBundle;
            break;
    }
}
