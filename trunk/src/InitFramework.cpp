//
//  InitFramework.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "UDPSendersManager.h"
#include "PipesManager.h"
#include "Sensor.h"
#include "SensorsManager.h"
#include "Receiver.h"

#undef USE_RECEIVER
//#ifdef USE_RECEIVER
scdf::Receiver *audioReceiver = NULL;
//#endif

void InitFramework()
{
    
    scdf::thePipesManager()->CreateReturnPipes();
    scdf::theSensorManager()->CreateAllSensors();
#ifdef USE_RECEIVER
    
    size_t rx_pkt_size=1024;
    unsigned int audio_buf_len = 256;
    unsigned int sensor_buf_len = 64;
    unsigned int graph_buf_len = 4*audio_buf_len;

    scdf::Receiver *p_receiver = new scdf::Receiver( rx_pkt_size,audio_buf_len,sensor_buf_len,graph_buf_len);
    audioReceiver = p_receiver;
    
    p_receiver->SetRemoteIp(192,168,1,52);
    p_receiver->SetLocalIp(192,168,1,107);
    p_receiver->SetPort(55000);
    p_receiver->Start();
#endif
}



