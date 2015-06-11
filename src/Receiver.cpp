//
//  Receiver.cpp
//  SCDF_Test
//
//  Created by nuno on 11/01/15.
//  Copyright (c) 2015 Marco Bertola. All rights reserved.
//

#include "Receiver.h"
#include <sys/types.h>

#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include "SCDF_OSCListener.h"


#define SENSORDATA_SIZE 2048 // 2 FIX
#define RECEPTION_MAX_BUFFER_LEN 2
namespace scdf {


Receiver::Receiver( size_t rx_pkt_size,unsigned int audio_buf_len,unsigned int sensor_buf_len,unsigned int graph_buf_len)
{

        listener = NULL;
        this->rx_pkt_size=rx_pkt_size;
        this->p_rx_buff=(char*)malloc(rx_pkt_size);
        memset(this->p_rx_buff,0,rx_pkt_size);
        this->audio_buf_len=audio_buf_len;
        this->sensor_buf_len=sensor_buf_len;
        this->graph_buf_len=graph_buf_len;
        this->p_graph_buff=(s_sample*)calloc(graph_buf_len,sizeof(s_sample));

}

Receiver::~Receiver()
{
     free(this->p_rx_buff);
    free(this->p_graph_buff);


}

    void Receiver::SetRemoteIp(unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4)
{
    this->remote_ip1=val1;
    this->remote_ip2=val2;
    this->remote_ip3=val3;
    this->remote_ip4=val4;
}

    void Receiver::SetLocalIp(unsigned int val1,unsigned int val2,unsigned int val3,unsigned int val4)
{
    this->local_ip1=val1;
    this->local_ip2=val2;
    this->local_ip3=val3;
    this->local_ip4=val4;
}
void Receiver::SetPort( unsigned int val1)
{
    this->port=val1;
}

/*void SCDF_Receiver_SetBuff(SCDF_Receiver_T *p_receiver, s_sample *val1)
{
    p_receiver->p_audio_buff=val1;
}*/

void Receiver::Start()
{
    printf("buffer size %d\n",this->sensor_buf_len);
    this->handle=scdf::ThreadUtils::CreateThread((start_routine)StartReceivingProcedure, this);
}

void Receiver::Stop()
{
    scdf::ThreadUtils::TerminateThread(this->handle);
}

void Receiver::StartReceivingProcedure(void *param)
{
    Receiver* p_receiver=((Receiver*)param);
    SCDFPacketListener* Rx_Listener_p=new SCDFPacketListener (p_receiver->sensor_buf_len,p_receiver->audio_buf_len);
    //static SensorData_T SCDF_rx_pkt;
    static char local_ip[16];
    static char remote_ip[16];
    unsigned long sock_buff_count=0;
    static IpEndpointName Rx_endpoint;
    int ret = 0;
    unsigned int i=0;
    static unsigned int k=0,m=0;
    unsigned int audio_buff_len=0;
    unsigned int graph_buff_len=0;
    unsigned int audio_graph_ratio=0;


    sprintf(local_ip,"%d.%d.%d.%d",p_receiver->local_ip1,p_receiver->local_ip2,p_receiver->local_ip3,p_receiver->local_ip4);
    sprintf(remote_ip,"%d.%d.%d.%d",p_receiver->remote_ip1,p_receiver->remote_ip2,p_receiver->remote_ip3,p_receiver->remote_ip4);

    printf("porta settata %d\n",p_receiver->port);
    Init_socket(local_ip,remote_ip,p_receiver->port,&(p_receiver->Sock_sd),&(p_receiver->localport_info));
    Rx_endpoint.address=( (p_receiver->remote_ip1 & 0xFF) <<24) | ( (p_receiver->remote_ip2 & 0xFF) <<16) | ( (p_receiver->remote_ip3 & 0xFF) <<8) | ( (p_receiver->remote_ip4 & 0xFF) <<0) ;//(remote_IP);
    Rx_endpoint.port=p_receiver->port;
     p_receiver->Get_audio_buff_len(&audio_buff_len);
     p_receiver->Get_graph_buff_len(&graph_buff_len);
    audio_graph_ratio=graph_buff_len/audio_buff_len;
    
    

    while(1)//(harvester->activated)
    {
        ret=receive(p_receiver->p_rx_buff, p_receiver->rx_pkt_size,local_ip,remote_ip,p_receiver->port,&(p_receiver->Sock_sd),&(p_receiver->fds),&(p_receiver->localport_info),&sock_buff_count);

        if (ret>0)
        {
            Rx_Listener_p->ProcessPacket(p_receiver->p_rx_buff,ret,Rx_endpoint);
             for (i=0;i<audio_buff_len;i++)
            {
                m=k%audio_graph_ratio;

                p_receiver->p_graph_buff[m*audio_buff_len+i]=(Rx_Listener_p->audio_data_buff_p)[i];//static_cast <float> (rand()) / static_cast <float> (RAND_MAX);


            }

            k++;

        }




        if(NULL!=p_receiver)
        {
             if(NULL!=p_receiver->listener)
             {

                     // for (i=0;i<graph_buff_len;i++)
                     // {
                      //    fprintf(stdout,"%f\n",p_receiver->p_graph_buff[i]);
                     // }

                 

                     if(m==(audio_graph_ratio-1))
                     {
                        // memset(p_receiver->p_graph_buff,0,graph_buff_len*sizeof(s_sample));
                         p_receiver->listener->draw_buffer(p_receiver->p_graph_buff,graph_buff_len);
                     }


             }
        }

        //printf("memset\n");

    }
}




  void  Receiver::Init_socket(char* ip_local,char* ip_remote,int port_id,int* SOCK_sd,struct sockaddr_in *localport_info)
{

    static unsigned int iOptVal_pkt;
    static unsigned int ActualOptVal_pkt;
    static int iOptLen_pkt = sizeof(ActualOptVal_pkt);
    unsigned int rcvTimeOut_pkt = 10;

    /*WSADATA w;	*/							/* Used to open Windows connection */
    /* Open windows connection */
    /*   if (WSAStartup(0x0202, &w) != 0)
     {
     printf("SCDF-> Could not create windows connection.\n");
     exit(0);
     }*/

    /* Open a datagram socket */
    *SOCK_sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (*SOCK_sd == -1)
    {
        printf("SCDF-> Could not create socket on port xxxx.\n, errno %d",errno);
        /*WSACleanup();*/
        exit(0);
        /*printf("SCDF-> Could not create socket on port xxxx.\n");*/
    }

    /* Clear out address struct */
    memset(localport_info,0, sizeof(struct sockaddr_in));
    /*memset(&remote_006,0, sizeof(struct sockaddr_in));*/

    /* Fill remote */
    /* remote_006.sin_family      = AF_INET;
     remote_006.sin_addr.s_addr = inet_addr(ip_remote);
     remote_006.sin_port        = htons(IF_C1_DATA_ANALYSIS_LISTEN_PORT);*/

    /* Fill local */
    localport_info->sin_family       = AF_INET;
    localport_info->sin_addr.s_addr  = inet_addr(ip_local);
    localport_info->sin_port         = htons(port_id);

    /*printf("local port init %d\n",localport_info->sin_port);*/

    /* Bind local address to socket */
    if (bind(*SOCK_sd,(struct sockaddr *)localport_info,sizeof(struct sockaddr_in)) == -1)
    {
        printf("SCDF-> Could not bind socket on port %d  : %d\n", port_id,errno);
        /*perror("SCDF-> Could not bind socket on port .\n");*/
        close(*SOCK_sd);
        /*  WSACleanup();*/


        exit(0);
    }

    /* Setting reception timeout ( NON_BLOCKING RECV/RECVFROM ) */
    if( rcvTimeOut_pkt>0 )
    {
        setsockopt(*SOCK_sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &rcvTimeOut_pkt, sizeof(rcvTimeOut_pkt));
    }



    /* Setting reception buffer */
    iOptVal_pkt= RECEPTION_MAX_BUFFER_LEN*SENSORDATA_SIZE;//2fix sizeof(SensorData_T);/*100 Ë per non perdere i pacchetti maybe to fix //10 * sizeof(t_usp_c1_data_msg);*/
    if ( setsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (char*)&iOptVal_pkt, sizeof(iOptVal_pkt))== -1 ){

        printf("SCDF-> Error on port %d at setsockopt(): %d\n",port_id, errno);
        /*WSACleanup();*/
        printf("SCDF-> Error ");
        exit(0);
    }
    if ( getsockopt( *SOCK_sd, SOL_SOCKET, SO_RCVBUF, (void*)&ActualOptVal_pkt, (socklen_t*)&iOptLen_pkt ) == -1){
        printf("SCDF-> Error on port %d at getsockopt(): %d\n",port_id, errno);
        /*WSACleanup();*/
        printf("SCDF-> Error ");
        exit(0);
    }
    if(iOptVal_pkt != ActualOptVal_pkt)
    {
        printf("SCDF-> Warning on port 5005 at getsockopt(): cannot fit the requested size buffer! desired %u actual %u\n",iOptVal_pkt,ActualOptVal_pkt);
    }

}

int Receiver::receive(char* line, int maxsize,char* ip_local_init,char* ip_remote_init,int port_id,int* SOCK_sd,fd_set *fds,struct sockaddr_in *localport_info,unsigned long *sock_buff_count)
{
    int ret = 0,ret_val=0;
    socklen_t socketlen = sizeof(struct sockaddr_in);
    static bool stopped= true;
    static int last_byte_received=0;
    /*static fd_set          fds;*/
    struct timeval  tv  = {5,00};

    FD_ZERO(fds);
    FD_SET( *SOCK_sd, fds );

    ret_val = select( *SOCK_sd+1, fds, NULL, NULL, &tv );

    if( ret_val == -1 )
    {
        printf("select on sd_006 failed! %d\n",errno);
        return -1111;
    }
    else if (ret_val == 0)
    {
        printf("select on sd_006 timedout! \n");
    }


    if( FD_ISSET(*SOCK_sd, fds) )
    {

        ret = recvfrom( *SOCK_sd, line, maxsize, 0 , (struct sockaddr *)localport_info, &socketlen);

        if ( (ret>0 && stopped==true) || (ret>0 && ret!=last_byte_received) )
        {
            printf("SCDF(%d)-> %d bytes received, local structurs size is %d bytes\n\n", ntohs(localport_info->sin_port), ret ,SENSORDATA_SIZE);
            stopped = false;
        }
        else if (ret<=0)
        {
            printf(" ERROR code from rcvfrom function%d\n",errno);
            stopped = true;
        }

#ifdef VERBOSE_UDP
        ioctl(*SOCK_sd,FIONREAD,sock_buff_count);
#endif

    }
    else
    {
        printf("SCDF-> timeout socket %d\n",port_id);
        ioctl(*SOCK_sd,FIONREAD,sock_buff_count);

        /* bDumpNavigation = true;*/

    }

    last_byte_received  = ret;
    return ret;
}

void Receiver::Get_audio_buff_len(unsigned int* line)
    {
        *line=this->audio_buf_len;
    }
void Receiver::Get_graph_buff_len(unsigned int* line)
    {
        *line=this->graph_buf_len;
    }
}


