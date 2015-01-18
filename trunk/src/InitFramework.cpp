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

//#define USE_RECEIVER

void InitFramework()
{
    
#ifdef USE_RECEIVER
    SCDF_Receiver_T *p_receiver=NULL;
    size_t rx_pkt_size=1024;
    unsigned int audio_buf_len = 512;
    unsigned int sensor_buf_len = 64;
    unsigned int graph_buf_len = 4*audio_buf_len;
#endif
    
    scdf::thePipesManager()->CreateReturnPipes();
    scdf::theSensorManager()->CreateAllSensors();
#ifdef USE_RECEIVER
    SCDF_Receiver_Init(&p_receiver, rx_pkt_size,audio_buf_len,sensor_buf_len,graph_buf_len);
    SCDF_Receiver_SetRemoteIp(p_receiver,192,168,1,55);
    SCDF_Receiver_SetLocalIp(p_receiver,192,168,1,54);
    SCDF_Receiver_SetPort(p_receiver,50005);
    SCDF_Receiver_Start(p_receiver);
#endif
}



