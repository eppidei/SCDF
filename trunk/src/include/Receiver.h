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




namespace scdf {
    
    class Receiver
    {
        public :
        
        class ReceiverListener
        {
            public :
                virtual void draw_buffer(s_sample *p_buff, unsigned int buff_len) = 0;
        };
        Receiver(){}
        
        Receiver( size_t rx_pkt_size,unsigned int audio_buf_len,unsigned int sensor_buf_len,unsigned int graph_buf_len);
        ~Receiver();
        
        void Start();
        void Stop();
        void SetRemoteIp( unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4);
        void SetLocalIp( unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4);
        void SetPort(unsigned int val1);
        
        void SetListener(ReceiverListener* _listener){listener = _listener;}
    
    
protected:
    
    ReceiverListener* listener;

    
        private :
        
        
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
            
        
        static void StartReceivingProcedure(void *param);
        static void Init_socket(char* ip_local,char* ip_remote,int port_id,int* SOCK_sd,struct sockaddr_in *localport_info);
        static int receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,int* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count);
        
    };

}

#endif /* defined(__SCDF_Test__Receiver__) */
