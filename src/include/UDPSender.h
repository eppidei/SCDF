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
#ifndef _RTP_MIDI_PROJECT
#include "Sensor.h"
#include "osc/OscOutboundPacketStream.h"

#include "cereal/archives/xml.hpp"
#include "cereal/access.hpp"
#include "cereal/types/string.hpp"
#else
#define SCDF_API
#endif

class UdpSocket;
class IpEndpointName;

namespace osc {
    class OutboundPacketStream;
}
namespace scdf
{
    class SCDF_API UDPSender
    {
        std::auto_ptr<UdpSocket> transmitSocket;
        std::vector<IpEndpointName*> endPoints;
        ThreadUtils::CustomMutex endpointsChanger;
        s_int32 portBase;
        std::string address;

        s_bool CheckCreateSender();
    public:
        UDPSender();
        void SendData(const s_char* data, s_int32 size, s_int32 endpointIndex);
        void SendData(const s_char* data, s_int32 size);
        std::size_t Receive(char *data, int size, s_int32 endpointIndex);
        int Bind(int endpointIndex);
        void InitEndpoints(s_int32 udp_base_num, s_int32 num_endpoints, std::string IP_address);
        s_int32 GetPort();
        std::string GetAddress();

        void SetPort(s_int32 p);
        void SetAddress(std::string addr);
        void SetNumEndpoints(s_int32 numEp);

#ifndef _RTP_MIDI_PROJECT
        friend class cereal::access;

		template <class Archive>
		void save( Archive & ar, std::uint32_t const version ) const
		{
			ar(CEREAL_NVP(address));
			ar(CEREAL_NVP(portBase));
			int numEndpoints = endPoints.size();
			ar(CEREAL_NVP(numEndpoints));
		}

		template <class Archive>
		void load( Archive & ar, std::uint32_t const version )
		{
			ar(CEREAL_NVP(address));
			ar(CEREAL_NVP(portBase));
			int numEndpoints;
			ar(CEREAL_NVP(numEndpoints));
			InitEndpoints(portBase, numEndpoints, address);
		}
#endif
    };

#ifndef _RTP_MIDI_PROJECT
    class UDPSenderHelperBase
    {
        friend class UDPSendersManager;
        std::auto_ptr<UDPSender> sender;
        ThreadUtils::CustomMutex activator;
        ThreadUtils::ThreadHandle handle;
        s_bool multiOutput, oscPackData;

        struct TempSensorData
        {
            s_int32 size;
            s_char *tempData;
            TempSensorData() : size(0), tempData(NULL) {}
            ~TempSensorData() { if (NULL!=tempData) free(tempData); }
            void PrepareBufferToSend(SensorData *data);
        };
        TempSensorData tempSensorData[scdf::NumTypes];

        void Init(s_int32 udpPortBase, std::string address);
        void SendData(std::vector<SensorData*> &senderData);
        void DoSendData(std::vector<SensorData*> &senderData);
        void DoMultiSendData(std::vector<SensorData*> &senderData);
        void OSCPackData(const std::vector<SensorData*> &sData, osc::OutboundPacketStream &oscData);
        void OSCSinglePackData(SensorData*, osc::OutboundPacketStream &oscData);
        void DoSendDataOSCPacked(std::vector<SensorData*> &senderData);
        void DoMultiSendDataOSCPacked(std::vector<SensorData*> &senderData);
        s_int32 CalculateOSCDataBufferSize(std::vector<SensorData*> &senderData);
        s_int32 GetPort();
        std::string GetAddress();

        UDPSenderHelperBase();
    public:
        void Activate(bool activate);
        void SendOnThread();
    };
#endif
}
#endif /* defined(__SCDF_Test__UDPSender__) */
