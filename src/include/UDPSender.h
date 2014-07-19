//
//  UDPSender.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__UDPSender__
#define __SCDF_Test__UDPSender__

#include "ThreadUtils.h"
#include "Sensor.h"

class UdpSocket;
class IpEndpointName;

namespace osc {
    class OutboundPacketStream;
}
namespace scdf
{
    class UDPSender
    {
//        std::auto_ptr<UdpTransmitSocket> transmitSocket;
        std::auto_ptr<UdpSocket> transmitSocket;
        std::auto_ptr<IpEndpointName> endPoint;
        void Release();
        std::string address;
    public:
        ~UDPSender() { Release(); }
        void SendData(s_char* data, s_int32 size);
        void SendDataOSCPacked(osc::OutboundPacketStream &oscData);
        s_int32 GetPort();
        std::string GetAddress();
        s_int32 Init(int udpp, std::string add);
    };
    
    class UDPSenderHelperBase
    {
        std::vector<UDPSender*> senders;
        
        void SendData();
        void DoSendData();
        void DoMultiSendData();
        void OSCPackData(SensorData*, osc::OutboundPacketStream &oscData);
        void DoSendDataOSCPacked();
        void DoMultiSendDataOSCPacked();
        
        ThreadUtils::CustomSemaphore freeSlot, canSend;
        ThreadUtils::ThreadHandle handle;
        struct TempSensorData
        {
            s_int32 size;
            s_char *tempData;
            TempSensorData() : size(0), tempData(NULL) {}
            ~TempSensorData() { if (NULL!=tempData) free(tempData); }
            void PrepareBufferToSend(SensorData *data);
        };
        TempSensorData tempSensorData[scdf::NumTypes];
    public:
        s_int32 GetPort();
        std::string GetAddress();
        bool activated;
        std::vector<SensorData*> senderData;
        
        UDPSenderHelperBase();
        ~UDPSenderHelperBase() { Release();}
        virtual s_int32 Init(std::vector<s_int32> udpPorts, std::string address);
        void Release();
        void SendOnThread();
        ThreadUtils::CustomSemaphore *EventFreeSlot() {return &freeSlot;}
        ThreadUtils::CustomSemaphore *EventCanSend() {return &canSend;}
    };
}
#endif /* defined(__SCDF_Test__UDPSender__) */
