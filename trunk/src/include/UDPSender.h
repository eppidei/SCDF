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

class UdpTransmitSocket;
class IpEndpointName;

namespace osc {
    class OutboundPacketStream;
}
namespace scdf
{
    class UDPSender
    {
        std::auto_ptr<UdpTransmitSocket> transmitSocket;
        std::auto_ptr<IpEndpointName> endPoint;
        void Init(int udpp, std::string add);
        void Release();
        std::string address;
    public:
        UDPSender() { Init(DEFAULT_UDP_PORT_BASE, DEFAULT_IP_ADDRESS); }
        UDPSender(int udpp, std::string add) { Init(udpp, add); }
        ~UDPSender() { Release(); }
        void SendData(s_char* data, s_int32 size);
        void SendDataOSCPacked(osc::OutboundPacketStream &oscData);
        s_int32 GetPort();
        std::string GetAddress();
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
    public:
        s_int32 GetPort();
        std::string GetAddress();
        bool activated;
        std::vector<SensorData*> senderData;
        
        UDPSenderHelperBase();
        ~UDPSenderHelperBase() { Release();}
        virtual void Init(std::vector<s_int32> udpPorts, std::string address);
        void Release();
        void SendOnThread();
        ThreadUtils::CustomSemaphore *EventFreeSlot() {return &freeSlot;}
        ThreadUtils::CustomSemaphore *EventCanSend() {return &canSend;}
    };
}
#endif /* defined(__SCDF_Test__UDPSender__) */
