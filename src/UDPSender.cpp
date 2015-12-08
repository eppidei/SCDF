//
//  UDPSender.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "UdpSocket.h"
#include "UDPSender.h"
#include "CustomPipe.h"
#include "PipesManager.h"
#include "Harvester.h"

using namespace scdf;

s_bool UDPSender::CheckCreateSender()
{
    if (NULL!=transmitSocket.get()) return true;
    
    try{
        transmitSocket.reset(new UdpSocket());
    }
    catch(std::runtime_error& e){
        LOGD("%s",e.what());
        return false;
    }
    return true;
}

UDPSender::UDPSender()
{
    CheckCreateSender();
}

void UDPSender::InitEndpoints(s_int32 udp_base_num, s_int32 num_endpoints, std::string IP_address)
{
    ThreadUtils::AutoLock kk(&endpointsChanger);
    address=IP_address;
    portBase=udp_base_num;
    endPoints.clear();
    for (int i=0;i<endPoints.size();++i)
        delete endPoints[i];
    endPoints.clear();
    
    endPoints.resize(num_endpoints);
    for (int i=0;i<num_endpoints;++i)
        endPoints[i]=new IpEndpointName(address.c_str(),udp_base_num+i);
}

s_int32 UDPSender::GetPort()
{
    return portBase;
}

std::string UDPSender::GetAddress()
{
    return address;
}

void UDPSender::SetPort(s_int32 port)
{
    InitEndpoints(port,endPoints.size(),address);
}

void UDPSender::SetAddress(std::string addr)
{
	InitEndpoints(portBase,endPoints.size(),addr);
}

void UDPSender::SetNumEndpoints(s_int32 numEp)
{
	InitEndpoints(portBase,numEp,address);
}

void UDPSender::SendData(const s_char* data, s_int32 size, s_int32 endpointIndex)
{
#ifdef LOG_UDP_SEND
	//LOGD("UDPSender send data\n");
#endif
    if (!CheckCreateSender()) return;
    if (endpointIndex>=endPoints.size()) return;
    ThreadUtils::AutoLock kk(&endpointsChanger);

    //LOGD("UDP SENDER - actually send %d bytes to %s [%d]",size,address.c_str(),portBase);
    try {
#ifndef _RTP_MIDI_PROJECT
        assert(osc::IsMultipleOf4(size));
#endif
        transmitSocket->SendTo(*endPoints[endpointIndex],data, size);
    }
    catch (const std::runtime_error& error)
    {
        LOGD("\nUDP SendTo failed with error: %s\n",error.what());
    }
    
}

std::size_t UDPSender::Receive(char *data, int size, s_int32 endpointIndex)
{
    static bool bound=false;
    if (!CheckCreateSender()) return 0;
    std::size_t ret=0;
    if (!bound)
    {
        bound=true;
        transmitSocket->Bind(*endPoints[endpointIndex]);
    }
    try {
        ret=transmitSocket->ReceiveFrom(*endPoints[endpointIndex],data, size);
    }
    catch (const std::runtime_error& error)
    {
        LOGD("\nUDP SendTo failed with error: %s\n",error.what());
    }
    return ret;
    
}
void UDPSender::SendData(const s_char* data, s_int32 size)
{
	SendData(data,size,0);
}


#ifndef _RTP_MIDI_PROJECT

void UDPSenderHelperBase::SendData(std::vector<SensorData*> &senderData)
{
	//LOGD("UDP SENDER - SEND DATA - select the way (multi: %d, osc: %d)!",multiOutput,oscPackData);

	if (multiOutput)
    {
        if (oscPackData)
            DoMultiSendDataOSCPacked(senderData);
        else
            DoMultiSendData(senderData);
    }
    else
    {
        if (oscPackData)
            DoSendDataOSCPacked(senderData);
        else
            DoSendData(senderData);
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

void UDPSenderHelperBase::DoSendData(std::vector<SensorData*> &senderData)
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
       LOGD("Pure UDP Send data");//: %s sensor send %d samples at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        tempSensorData[senderData[i]->type].PrepareBufferToSend(senderData[i]);
        sender->SendData(tempSensorData[senderData[i]->type].tempData,tempSensorData[senderData[i]->type].size, 0);
    }
}

void UDPSenderHelperBase::DoSendDataOSCPacked(std::vector<SensorData*> &senderData)
{
#ifdef LOG_SENDER_DATA
   LOGD("Send OSC packed data");//: %s sensor send %d samples at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
    s_int32 oscBufferSize=CalculateOSCDataBufferSize(senderData);
    s_char buffer[oscBufferSize];
    osc::OutboundPacketStream oscData( buffer, oscBufferSize);
    OSCPackData(senderData, oscData);
    sender->SendData(oscData.Data(), oscData.Size(),0);
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

s_int32 UDPSenderHelperBase::CalculateOSCDataBufferSize(std::vector<SensorData*> &senderData)
{
    s_int32 size=0;
    for (s_int32 i=0;i<senderData.size();++i)
        size+=CalculateOSCDataSingleBufferSize(senderData[i]);
    return size;
}

void UDPSenderHelperBase::DoMultiSendDataOSCPacked(std::vector<SensorData*> &senderData)
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
        LOGD("MultiSend OSC packed data");//: %s sensor send %d samples at rate %d\n", SensorTypeString[i].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        
        s_int32 oscSingleBufferSize=CalculateOSCDataSingleBufferSize(senderData[i]);
        s_char buffer[oscSingleBufferSize];
        osc::OutboundPacketStream oscData( buffer, oscSingleBufferSize);
        OSCSinglePackData(senderData[i], oscData);
        sender->SendData(oscData.Data(), oscData.Size(), senderData[i]->type);
    }
}

void UDPSenderHelperBase::DoMultiSendData(std::vector<SensorData*> &senderData)
{
    for (int i=0;i<senderData.size();++i)
    {
#ifdef LOG_SENDER_DATA
         LOGD("Pure UDP MultiSend data");//: %s sensor send %d frames at rate %d\n", SensorTypeString[senderData[i]->type].c_str(), senderData[i]->num_frames, senderData[i]->rate);
#endif
        if (0==senderData[i]->num_frames) continue;
        tempSensorData[senderData[i]->type].PrepareBufferToSend(senderData[i]);
        sender->SendData(tempSensorData[senderData[i]->type].tempData,tempSensorData[senderData[i]->type].size, senderData[i]->type);
    }
}

void UDPSenderHelperBase::SendOnThread()
{
	LOGD("UDP SENDER - UDPSenderHElperBase::SendOnThread() - acquiring lock...");
    ThreadUtils::AutoLock kk(&activator);
    LOGD("UDP SENDER - UDPSenderHElperBase::SendOnThread() - ACQUIRED LOCK, wait for harvest...");

    Harvester::Instance()->WaitForHarvest();

    try{

    	LOGD("UDP SENDER - UDPSenderHelperBase - wait for harvest over... SendOnThread()");

        std::vector<SensorData*> *buffer=Harvester::Instance()->syncDataQueue.front();
        Harvester::Instance()->syncDataQueue.pop();
#ifdef LOG_SEM
        LOGD("Sender data queue size :%d\n",Harvester::Instance()->syncDataQueue.size());
#endif
        if (NULL==buffer) return;
        SendData(*buffer);
        for(int i=0;i<buffer->size();++i)
            delete (*buffer)[i];
        delete buffer;
    }
    catch(std::runtime_error& e)
    {
        std::string s(e.what());
        s+=std::string("/n");
        LOGD("%s",s.c_str());
    }
}

static void UDPSenderHelperProcedure(void *param)
{
	UDPSenderHelperBase *sender=((UDPSenderHelperBase*)param);

	//LOGD("UDP SENDER HELPER Procedure start");

    while(1)
    {
    	//LOGD("UDP SENDER HELPER Send on thread");
        sender->SendOnThread();
    }
}

UDPSenderHelperBase::UDPSenderHelperBase() : sender(new UDPSender()), multiOutput(true), oscPackData(true)
{
	LOGD("UDP SENDER HELPER BASE Creation");
    activator.Lock();
    handle=ThreadUtils::CreateThread((start_routine)UDPSenderHelperProcedure, this);
}

s_int32 UDPSenderHelperBase::GetPort()
{
    return sender->GetPort();
}

std::string UDPSenderHelperBase::GetAddress()
{
    return sender->GetAddress();
}

void UDPSenderHelperBase::Init(s_int32 udpPortBase, std::string address)
{
    sender->InitEndpoints(udpPortBase, NumTypes, address);
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
        << osc::BeginMessage( std::string(sensorTag+timeIDTag).c_str() ) << (osc::int64)data->timeid << osc::EndMessage
        << osc::BeginMessage( std::string(sensorTag+dataTag).c_str() ) << osc::Blob(data->data, data->num_frames*data->num_channels*sizeof(s_sample))<< osc::EndMessage
        << osc::BeginMessage( std::string(sensorTag+timestampsTag).c_str() ) << osc::Blob(data->timestamp,numTimestamps*sizeof(s_uint64)) << osc::EndMessage;
    }
    oscData << osc::EndBundle;
}

void UDPSenderHelperBase::OSCSinglePackData(SensorData *data, osc::OutboundPacketStream &oscData)
{
    std::vector<SensorData*> tempData;
    tempData.push_back(data);
    
    OSCPackData(tempData,oscData);
}

void UDPSenderHelperBase::Activate(s_bool active)
{
	LOGD("UDP SENDER - UDPSenderHelperBase::Activate(%d)",active);

    if (active){
    	LOGD("UDP SENDER - Sender helper base activate: UNLOCKING activator...");
        activator.Unlock();
    }
    else{
        Harvester::Instance()->SendingQueuePushBuffer(NULL);
        LOGD("UDP SENDER - Activate() : Activator LOCK...");
        activator.Lock();
    }
}

#endif

