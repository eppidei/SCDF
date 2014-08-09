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
        endPoint.reset(new IpEndpointName(add.c_str(), udpp));
        //transmitSocket.reset(new UdpTransmitSocket((*(endPoint.get()))));
        transmitSocket.reset(new UdpSocket());
    } catch(std::runtime_error& e)
    {
        LOGD("%s",e.what());
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
	//endPoint.reset();
}

void UDPSender::SendData(const s_char* data, s_int32 size)
{
#ifdef LOG_UDP_SEND
	LOGD("UDPSender send data\n");
#endif
	//transmitSocket->Send(data, size);
    transmitSocket->SendTo(*endPoint.get(),data, size);
}

/*void UDPSender::SendDataOSCPacked(osc::OutboundPacketStream &oscData)
{
#ifdef LOG_UDP_SEND
	LOGD("UDP send data osc packed\n");
#endif
    //transmitSocket->Send(oscData.Data(), oscData.Size());
    transmitSocket->SendTo(*endPoint.get(), oscData.Data(), oscData.Size());
}*/

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

void UDPSenderHelperBase::TempSensorData::PrepareBufferToSend(SensorData *sData)
{
    s_int32 numTimestamps=2;
    if (AudioInput!=sData->type) {
        numTimestamps=sData->num_frames;
        //LOGD("Prepare buffer to send for %d. timestamps num = %d",sData->type,numTimestamps);
    }

    s_int32 samplesBlockSize = sData->num_frames*sData->num_channels*sizeof(s_sample);
    s_int32 timestampsBlockSize = numTimestamps*sizeof(s_uint64);
    s_int32 unusedPointersBlockSize = 0;//sizeof(s_sample*) + sizeof(s_uint64*);
    s_int32 sensorDataSize=sizeof(SensorData) -unusedPointersBlockSize + samplesBlockSize + timestampsBlockSize;
    
    if (size!=sensorDataSize)
    {
    	//LOGD("type %d current size: %d, new size: %d",sData->type,size,sensorDataSize);
    	size=sensorDataSize;
        if (NULL!=tempData) {
        	free(tempData);
        }
        tempData=(char*)malloc(size);
    }

    s_char* tempData_samples = tempData + sizeof(SensorData) - unusedPointersBlockSize ;
    s_char* tempData_timestamps = tempData_samples + samplesBlockSize;
    //LOGD("tempdata %d, samples offset %d, timestamps offset %d",tempData,tempData_samples,tempData_timestamps);
    memcpy(tempData, sData, sizeof(SensorData));
    memcpy(tempData_samples,sData->data, samplesBlockSize);
    memcpy(tempData_timestamps,sData->timestamp, timestampsBlockSize);
}

void UDPSenderHelperBase::DoSendData()
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
       // LOGD("Send data: %s sensor send %d samples at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        tempSensorData[senderData[i]->type].PrepareBufferToSend(senderData[i]);
        senders[0]->SendData(tempSensorData[senderData[i]->type].tempData,tempSensorData[senderData[i]->type].size);
    }
}

void UDPSenderHelperBase::DoMultiSendData()
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
       // LOGD("MultiSend data: %s sensor send %d frames at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        tempSensorData[senderData[i]->type].PrepareBufferToSend(senderData[i]);
        senders[senderData[i]->type]->SendData(tempSensorData[senderData[i]->type].tempData,tempSensorData[senderData[i]->type].size);
    }
}

void UDPSenderHelperBase::DoSendDataOSCPacked()
{
#ifdef LOG_SENDER_DATA
    //LOGD("Send OSC packed data: %s sensor send %d samples at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
    s_int32 oscBufferSize=CalculateOSCDataBufferSize();
    s_char buffer[oscBufferSize];
    osc::OutboundPacketStream oscData( buffer, oscBufferSize);
    OSCPackData(senderData, oscData);
    senders[0]->SendData(oscData.Data(), oscData.Size());
}

s_int32 CalculateOSCDataSingleBufferSize(SensorData *data)
{
    s_int32 numTimestamps=2;
    if (AudioInput!=data->type)
        numTimestamps=data->num_frames;
    
    s_int32 samplesBlockSize = data->num_frames*data->num_channels*sizeof(s_sample);
    s_int32 timestampsBlockSize = numTimestamps*sizeof(s_uint64);
    s_int32 sensorDataStructMembersSize=sizeof(data->rate) + sizeof(data->timeid) + sizeof(data->num_frames) + sizeof(data->num_channels);
    return (sensorDataStructMembersSize + samplesBlockSize + timestampsBlockSize + 512);
}

s_int32 UDPSenderHelperBase::CalculateOSCDataBufferSize()
{
    s_int32 size=0;
    for (s_int32 i=0;i<senderData.size();++i)
        size+=CalculateOSCDataSingleBufferSize(senderData[i]);
    return size;
}

void UDPSenderHelperBase::DoMultiSendDataOSCPacked()
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
        LOGD("MultiSend OSC packed data: %s sensor send %d samples at rate %d\n", SensorTypeString[i].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        
        s_int32 oscSingleBufferSize=CalculateOSCDataSingleBufferSize(senderData[i]);
        s_char buffer[oscSingleBufferSize];
        osc::OutboundPacketStream oscData( buffer, oscSingleBufferSize);
        OSCSinglePackData(senderData[i], oscData);
        senders[senderData[i]->type]->SendData(oscData.Data(), oscData.Size());
    }
}

void UDPSenderHelperBase::SendOnThread()
{
    EventCanSend()->Wait();
    if (!activated) return;
    try{
        SendData();
    }
    catch(std::runtime_error& e)
    {
        std::string s(e.what());
        s+=std::string("/n");
        LOGD("%s",s.c_str());
        //assert(false);
    }
    
    EventFreeSlot()->Set();
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

s_int32 UDPSenderHelperBase::Init(std::vector<s_int32> udpPorts, std::string address)
{
    assert(udpPorts.size()!=0);
    for (int i=0;i<udpPorts.size();++i)
    {
        UDPSender *s=new UDPSender();
        if (1==s->Init(udpPorts[i],address))
            senders.push_back(s);
    }
    if (0==senders.size())
        return 0;
    activated=true;
    handle=ThreadUtils::CreateThread((start_routine)UDPSenderHelperProcedure, this);
    return 1;
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

void UDPSenderHelperBase::OSCPackData(const std::vector<SensorData*> &sData, osc::OutboundPacketStream &oscData)
{
    std::string rateTag("/Rate");
    std::string channelsTag("/Channels");
    std::string samplesTag("/Frames");
    std::string timeIDTag("/TimeID");
    std::string dataTag("/Data");
    std::string timestampsTag("/Timestamps");
    
    oscData << osc::BeginBundle();
    for (int i=0;i<sData.size();++i)
    {
        if (0==sData[i]->num_frames) continue;
        
        SensorData *data=sData[i];
        s_int32 numTimestamps=2;
        if (AudioInput!=data->type)
        numTimestamps=data->num_frames;
        
        std::string sensorTag=std::string("/") + SensorTypeString[i];
        
        oscData << osc::BeginMessage( std::string(sensorTag+rateTag).c_str() ) << data->rate << osc::EndMessage
        << osc::BeginMessage( std::string(sensorTag+channelsTag).c_str() ) << data->num_channels << osc::EndMessage
        << osc::BeginMessage( std::string(sensorTag+samplesTag).c_str() ) << data->num_frames << osc::EndMessage
        << osc::BeginMessage( std::string(sensorTag+timeIDTag).c_str() ) << (osc::int64)data->timeid << osc::EndMessage;
        //<< osc::BeginMessage( std::string(sensorTag+dataTag).c_str() ) << osc::Blob(data->data, data->num_frames*data->num_channels*sizeof(s_sample))<< osc::EndMessage
        
        oscData << osc::BeginMessage( std::string(sensorTag+dataTag).c_str() )
        << osc::BeginArray;
        for (int i=0;i<data->num_frames*data->num_channels;++i)
            oscData << data->data[i];
        oscData << osc::EndArray << osc::EndMessage;
        
            
        oscData << osc::BeginMessage( std::string(sensorTag+timestampsTag).c_str() ) << osc::Blob(data->timestamp,numTimestamps*sizeof(s_uint64)) << osc::EndMessage;
    }
    oscData << osc::EndBundle;
}

void UDPSenderHelperBase::OSCSinglePackData(SensorData *data, osc::OutboundPacketStream &oscData)
{
    std::vector<SensorData*> tempData;
    tempData.push_back(data);
    
    OSCPackData(tempData,oscData);
    
/*    s_int32 numTimestamps=2;
    if (AudioInput!=data->type)
        numTimestamps=data->num_frames;

    oscData << osc::BeginBundle()
    << osc::BeginMessage( "/Sensor type") << data->type << osc::EndMessage
    << osc::BeginMessage( "/Rate"	) << data->rate << osc::EndMessage
    << osc::BeginMessage( "/Channels"	) << data->num_channels << osc::EndMessage
    << osc::BeginMessage( "/Num samples") << data->num_frames << osc::EndMessage
    << osc::BeginMessage( "/Time ref") << (osc::int64)data->timeid << osc::EndMessage
    << osc::BeginMessage( "/Data:"	) << osc::Blob(data->data, data->num_frames*data->num_channels*sizeof(s_sample))<< osc::EndMessage
    << osc::BeginMessage( "/Timestamp") << osc::Blob(data->timestamp,numTimestamps*sizeof(s_uint64)) << osc::EndMessage;
    oscData << osc::EndBundle;*/
}
