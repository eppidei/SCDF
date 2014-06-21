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
        auto_ptr<UdpTransmitSocket> transmitSocket;
        auto_ptr<IpEndpointName> endPoint;
        void Init(int udpp, string add);
        void Release();
    public:
        UDPSender() { Init(TEST_UDP_PORT_BASE, TEST_IP_ADDRESS); }
        UDPSender(int udpp, string add) { Init(udpp, add); }
        ~UDPSender() { Release(); }
        void SendData(s_char* data, s_int32 size);
        void SendDataOSCPacked(osc::OutboundPacketStream &oscData);
    };
    
    class UDPSenderHelperBase
    {
        vector<UDPSender*> senders;
        ThreadUtils::CustomSemaphore freeSlot, canSend;
        
        void SendData();
        void DoSendData();
        void DoMultiSendData();
        void OSCPackData(SensorData*, osc::OutboundPacketStream &oscData);
        void DoSendDataOSCPacked();
        void DoMultiSendDataOSCPacked();
    public:
        bool activated;
        vector<SensorData*> senderData;
        
        UDPSenderHelperBase();
        virtual void Init(vector<s_int32> udpPorts, string address);
        void SendOnThread();
        ThreadUtils::CustomSemaphore *EventFreeSlot() {return &freeSlot;}
        ThreadUtils::CustomSemaphore *EventCanSend() {return &canSend;}
    };
}
#endif /* defined(__SCDF_Test__UDPSender__) */
