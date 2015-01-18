//
//  Receiver.h
//  SCDF_Test
//
//  Created by nuno on 11/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__Receiver__
#define __SCDF_Test__Receiver__

//#include <stdio.h>

#include "TypeDefinitions.h"
#include "ThreadUtils.h"
#include <netinet/in.h>
/********************** UDP RECEIVER DEFINES ***********************************/

//#define MAX_BUFFER_LEN (4096)
//#define MAX_BUFFER_TIMESTAMP (128)
#define RECEPTION_MAX_BUFFER_LEN (4)
//#define MAX_CHAR_BUFF_LEN (18000)
#undef VERBOSE_OSC
#undef VERBOSE_UDP
#undef VERBOSE_THREAD


typedef enum SensorType{
    Invalid=-1,
    Accelerometer,
    Gyroscope,
    Magnetometer,
    Proximity,
    Light,
    AudioInput,
    NumTypes }SensorType_T ;

//
//#pragma pack(push)  /* push current alignment to stack */
//#pragma pack(1)     /* set alignment to 1 byte boundary */
//typedef struct SensorData {
//    SensorType_T type;
//    s_int32 rate;           /* time of the sensor reading as reported by the sensor*/
//    s_uint64 timeid;                /* will be the same for all data harvested in the same call*/
//    s_int32 num_frames;
//    s_int32 numChannels;
//    s_uint64 *ptimestamp;
//    s_sample *pdata;
//    s_sample data[MAX_BUFFER_LEN]; /*uint32 per nthol*/
//    s_uint64 timestamp[MAX_BUFFER_TIMESTAMP];
//} SensorData_T;
//
//#pragma pack(pop)


typedef struct SCDF_Receiver_S SCDF_Receiver_T;

struct SCDF_Receiver_S
{
    int Sock_sd;
    fd_set fds;
    struct sockaddr_in localport_info;
    int port;
    unsigned int local_ip1;
    unsigned int local_ip2;
    unsigned int local_ip3;
    unsigned int local_ip4;
    unsigned int remote_ip1;
    unsigned int remote_ip2;
    unsigned int remote_ip3;
    unsigned int remote_ip4;
    size_t rx_pkt_size;
    unsigned int audio_buf_len;
    unsigned int sensor_buf_len;
    unsigned int graph_buf_len;
    char *p_rx_buff;
    s_sample *p_graph_buff;
    scdf::ThreadUtils::ThreadHandle handle;
};

int SCDF_Receiver_Init(SCDF_Receiver_T **p_receiver, size_t rx_pkt_size,unsigned int audio_buf_len,unsigned int sensor_buf_len,unsigned int graph_buf_len);
void SCDF_Receiver_Release(SCDF_Receiver_T *p_receiver);
void SCDF_Receiver_Start(SCDF_Receiver_T *p_receiver);
void SCDF_Receiver_Stop(SCDF_Receiver_T *p_receiver);
void SCDF_Receiver_SetRemoteIp(SCDF_Receiver_T *p_receiver, unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4);
void SCDF_Receiver_SetLocalIp(SCDF_Receiver_T *p_receiver, unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4);
void SCDF_Receiver_SetPort(SCDF_Receiver_T *p_receiver, unsigned int val1);

#endif /* defined(__SCDF_Test__Receiver__) */
