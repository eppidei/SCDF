//
//  FuzAppleMIDIChannel.cpp
//  RtpMIDISession
//
//  Created by andrea scuderi on 09/01/16.
//  Copyright © 2016 fuzone. All rights reserved.
//

#include "FuzAppleMIDIChannel.hpp"
#include "FuzAppleMIDIDefines.h"
#include <stdlib.h>
#include <string.h>

//#include <time.h>
#include <mach/clock.h>
#include <mach/mach.h>

#include "Logging.h"


//ADE_UINT64_T htonll(ADE_UINT64_T val)
//{
//    union
//    {
//        ADE_UINT32_T lw[2];
//        ADE_UINT64_T llw;
//    } u;
//    u.lw[0]=htonl(val>>32);
//    u.lw[1]=htonl(val & 0xFFFFFFFFULL);
//    return u.llw;
//}
//
//ADE_UINT64_T ntohll(ADE_UINT64_T val)
//{
//    union
//    {
//        ADE_UINT32_T lw[2];
//        ADE_UINT64_T llw;
//    } u;
//    u.llw = val;
//    
//    return  ( (ADE_UINT64_T) ntohl(u.lw[0])<<32) | (ADE_UINT64_T)ntohl (u.lw[1]);
//}

void AppleMIDIChannel::Init(char *p_LocalIp, char *p_RemoteIp, int DstPort, int SrcPort, int *p_TxDesc, int *p_RxDesc, std::string p_ChName, unsigned int SenderSrc)
{
    if (DoInit()<0)
    {
        LOGD("Error creating channel");
        return;
    }
    ConfigUDPReceiver(p_LocalIp,p_RemoteIp, SrcPort, DstPort);
    ConfigUDPSender(p_LocalIp,p_RemoteIp, DstPort, SrcPort);
    GetSocketDescriptors(p_TxDesc,p_RxDesc);
    SetSenderSource(SenderSrc);
    SetName(p_ChName.c_str());
}

int AppleMIDIChannel::DoInit()
{
//    APPLEMIDI_CH_T *p_this=NULL;
//    
    channel=(APPLEMIDI_CH_T*)calloc(1,sizeof(APPLEMIDI_CH_T));
    
    if (channel!=NULL)
    {
        ADE_UdpSender_Init(&(channel->p_Sender));
        ADE_UdpReceiver_Init(&(channel->p_Receiver));
        return 0;
    }
    else
    {
        return -1;
    }
}

void AppleMIDIChannel::ConfigUDPSender(char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort)
{
    ADE_UdpSender_CloneSocket(channel->p_Sender,(ADE_UDPSENDER_T*)channel->p_Receiver);
}

void AppleMIDIChannel::ConfigUDPReceiver(char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort)
{
    ADE_UdpReceiver_CreateSocket(channel->p_Receiver);
    ADE_UdpReceiver_SetReuseAddress(channel->p_Receiver);
    //if (ADE_UdpReceiver_SetReusePort(p_ch->p_Receiver)==-1)
    //{
    //
    //}
    ADE_UdpReceiver_SetLocal(channel->p_Receiver,p_LocalIp,DstPort);
    ADE_UdpReceiver_SetRemote(channel->p_Receiver,p_RemoteIp,SrcPort);
    //ADE_UdpReceiver_SetNonBlocking(p_ch->p_Receiver);
}


void DoPrepareAcceptInvitationPacket(APPLEMIDI_T *p_pkt_tx,int IniziatorToken,unsigned int  SenderSource,char *p_name,int *p_actual_len)
{
    p_pkt_tx->header.Signature=htons(0xffff);
    p_pkt_tx->header.Command=htons(InvitationAccepted);
    p_pkt_tx->Invitation.ProtVersion=htonl(2);
    p_pkt_tx->Invitation.IniziatorToken=htonl(IniziatorToken);
    p_pkt_tx->Invitation.SenderSource=htonl(SenderSource);
    strcpy(p_pkt_tx->Invitation.name,p_name);
    p_pkt_tx->Invitation.name_length=strlen(p_name);
    *p_actual_len=4+4+4+4+p_pkt_tx->Invitation.name_length+1;
}

void AppleMIDIChannel::PrepareAcceptInvitationPacket(int *p_actual_len)
{
    DoPrepareAcceptInvitationPacket(&(channel->AppleMidiTx),channel->IniziatorToken,channel->SenderSource,channel->name,p_actual_len);
}

void DoParsePacket(char *p_buff, APPLEMIDI_T* p_AppleMIDIPkt)
{
    APPLEMIDI_T * p_int_buff=NULL;
    
    p_int_buff=(APPLEMIDI_T *)p_buff;
    short Command,Signature;
    
    Command=ntohs(p_int_buff->header.Command);
    Signature=ntohs(p_int_buff->header.Signature);
    
    p_AppleMIDIPkt->header.Command=Command;
    p_AppleMIDIPkt->header.Signature=Signature;
    
    
    if (Command==Invitation || Command==InvitationRejected || Command==InvitationAccepted )
    {
        
        p_AppleMIDIPkt->Invitation.ProtVersion=ntohl(p_int_buff->Invitation.ProtVersion);
        p_AppleMIDIPkt->Invitation.IniziatorToken=ntohl(p_int_buff->Invitation.IniziatorToken);
        p_AppleMIDIPkt->Invitation.SenderSource=ntohl(p_int_buff->Invitation.SenderSource);
        strcpy(p_AppleMIDIPkt->Invitation.name,p_int_buff->Invitation.name);
        
    }
    else if (Command==Synchronization)
    {
        p_AppleMIDIPkt->Synchronization.SenderSSRC=ntohl(p_int_buff->Synchronization.SenderSSRC);
        p_AppleMIDIPkt->Synchronization.Count=ntohl(p_int_buff->Synchronization.Count);
        p_AppleMIDIPkt->Synchronization.Padding=ntohl(p_int_buff->Synchronization.Padding);
        p_AppleMIDIPkt->Synchronization.TimeStamp1=ntohll(p_int_buff->Synchronization.TimeStamp1);
        p_AppleMIDIPkt->Synchronization.TimeStamp2=ntohll(p_int_buff->Synchronization.TimeStamp2);
        p_AppleMIDIPkt->Synchronization.TimeStamp3=ntohll(p_int_buff->Synchronization.TimeStamp3);
        
    }
    else if (Command==ReceiverFeedback)
    {
        p_AppleMIDIPkt->RecvFeedBack.SenderSSRC=ntohl(p_int_buff->RecvFeedBack.SenderSSRC);
        p_AppleMIDIPkt->RecvFeedBack.SequenceNumber=ntohl(p_int_buff->RecvFeedBack.SequenceNumber);
        
    }
    else
    {
        LOGD("APPLE COMMAND UNKNOWN !\n");
//        fprintf(stderr, "APPLE COMMAND UNKNOWN !\n");
    }
    
}

void AppleMIDIChannel::ParsePacket()
{
    ssize_t n_recv_bytes;
    ADE_UdpReceiver_Recv(channel->p_Receiver,&n_recv_bytes);
    DoParsePacket((char*)channel->p_Receiver->RecvBuff.p_Buff,&(channel->AppleMidiRx));
}

void AppleMIDIChannel::GetSocketDescriptors(int *p_TxDesc, int *p_RxDesc)
{
    ADE_UdpSender_GetDescriptor(channel->p_Sender,p_TxDesc);
    ADE_UdpReceiver_GetDescriptor(channel->p_Receiver,p_RxDesc);
}

void AppleMIDIChannel::SetSenderSource(unsigned int SenderSrc)
{
    channel->SenderSource=SenderSrc;
}

void AppleMIDIChannel::SetIniziatorToken(int Token)
{
    channel->IniziatorToken=Token;
}

void AppleMIDIChannel::SetName(const char *p_name)
{
    strcpy(channel->name,p_name);
}

void AppleMIDIChannel::SendPkt(size_t BuffSize)
{
    ssize_t SentSize;
    ADE_UdpSender_SetExternallBuff(channel->p_Sender,&(channel->AppleMidiTx), BuffSize);
    ADE_UdpSender_SendExternalBuff(channel->p_Sender,BuffSize, &SentSize);
}

void AppleMIDIChannel::GetReceivedCommand(short *p_Command)
{
    *p_Command=channel->AppleMidiRx.header.Command;
}

void AppleMIDIChannel::Release()
{
    ADE_UdpSender_Release(channel->p_Sender);
    ADE_UdpReceiver_Release(channel->p_Receiver);
    free(channel);
}
void AppleMIDIChannel::GetReceivedToken(int *p_Token)
{
    *p_Token=channel->AppleMidiRx.Invitation.IniziatorToken;
}

void SetTimestamp(APPLEMIDI_CH_T *p_ch)
{    
//    struct timespec monotime;
//    clock_gettime(CLOCK_MONOTONIC, &monotime);
//    p_ch->Timestamp=monotime.tv_nsec;
    
    clock_serv_t cclock;
    mach_timespec_t mts;
    
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    p_ch->Timestamp=mts.tv_nsec;
    mach_port_deallocate(mach_task_self(), cclock);
}

void DoPrepareSyncPacket(APPLEMIDI_T *p_pkt_tx,APPLEMIDI_T *p_pkt_rx,unsigned int SenderSource,ADE_UINT64_T tstamp,char count,int padding,int *p_actual_len)
{
    int temp=( count << 24 | (0x00FFFFFF & padding) );
    
    p_pkt_tx->header.Signature=htons(0xffff);
    p_pkt_tx->header.Command=htons(Synchronization);
    p_pkt_tx->Synchronization.SenderSSRC=htonl(SenderSource);
    // p_pkt_tx->Synchronization.Count=count;//htonl(IniziatorToken);
    // p_pkt_tx->Synchronization.Padding=0x000;//htonl(SenderSource);
    p_pkt_tx->Synchronization.word=htonl(temp);
    if (count==0)
    {
        p_pkt_tx->Synchronization.TimeStamp1= p_pkt_rx->Synchronization.TimeStamp1;
        p_pkt_tx->Synchronization.TimeStamp2=tstamp;
        p_pkt_tx->Synchronization.TimeStamp3=p_pkt_rx->Synchronization.TimeStamp3;
    }
    else if (count==1)
    {
        p_pkt_tx->Synchronization.TimeStamp1= p_pkt_rx->Synchronization.TimeStamp1;
        p_pkt_tx->Synchronization.TimeStamp2=p_pkt_rx->Synchronization.TimeStamp2;
        p_pkt_tx->Synchronization.TimeStamp3=tstamp;
    }
    else if (count==2)
    {
        p_pkt_tx->Synchronization.TimeStamp1= tstamp;
        p_pkt_tx->Synchronization.TimeStamp2=p_pkt_rx->Synchronization.TimeStamp2;
        p_pkt_tx->Synchronization.TimeStamp3=p_pkt_rx->Synchronization.TimeStamp3;
    }
    
    *p_actual_len=4+4+4+8+8+8;
}

void AppleMIDIChannel::PrepareSyncPacket(int *p_actual_len)
{
    int padding = 0x0000;
    SetTimestamp(channel);
    DoPrepareSyncPacket(&(channel->AppleMidiTx),&(channel->AppleMidiRx),channel->SenderSource,channel->Timestamp,channel->AppleMidiRx.Synchronization.Count,padding,p_actual_len);
}

void AppleMIDIChannelsManager::ConnectionProcedure(void *param)
{
    AppleMIDIChannelsManager *manager=(AppleMIDIChannelsManager*)param;
    int i;
    for (i=0;i<manager->active_read_fds;i++)
    {
        FD_SET(manager->my_read_fds[i], &manager->readset);
        // maxfd = (maxfd>my_read_fds[i])?maxfd:my_read_fds[i];
    }
    timeval tv  = {10,0}; /* SU LINUX TV VA SEMPRE REINIZIALIZZATO*/
    int result = select(manager->maxfd+1, &manager->readset,NULL , NULL, &tv);//&writeset
    if (result == -1) {
        LOGD("SELECT ERROR ERRNO %d\n",errno);
//        fprintf(stderr," SELECT ERROR ERRNO %d\n",errno);
    }
    else if (result == 0) {
        LOGD("SELECT TIMED OUT\n");
//        fprintf(stderr," SELECT TIMED OUT\n");
    }
    else {
//        sel_cnt++;
        for (i=0; i<manager->active_read_fds; i++)
        {
            if (FD_ISSET(manager->my_read_fds[i], &manager->readset))
            {
                short rx_command, ch_state;
                int rx_token, tx_pkt_len;
                if (i==0)
                {
                    LOGD("CTRL CHANNELL RECEIVED\n");
                    manager->GetControlChannel()->ParsePacket();
                    manager->GetControlChannel()->GetReceivedCommand(&rx_command);
                    manager->GetControlChannel()->GetReceivedToken(&rx_token);
                    manager->GetControlChannel()->SetIniziatorToken(rx_token);
                    ch_state=rx_command;
                    if (ch_state==Invitation)
                    {
                        manager->GetControlChannel()->PrepareAcceptInvitationPacket(&tx_pkt_len);
                        manager->GetControlChannel()->SendPkt(tx_pkt_len);
                        ch_state=InvitationAccepted;
                    } else if (ch_state==ReceiverFeedback)
                    {
                        LOGD("CTRL CHANNELL FEEDBACK\n");
                    }
                }
                else if (i==1)
                {
                    
                    LOGD("STREAM CHANNELL RECEIVED\n");
                    manager->GetDataChannel()->ParsePacket();
                    manager->GetDataChannel()->GetReceivedCommand(&rx_command);
                    manager->GetDataChannel()->GetReceivedToken(&rx_token);
                    manager->GetDataChannel()->SetIniziatorToken(rx_token);
                    ch_state=rx_command;
                    if (ch_state==Invitation)
                    {
                        manager->GetDataChannel()->PrepareAcceptInvitationPacket(&tx_pkt_len);
                        manager->GetDataChannel()->SendPkt(tx_pkt_len);
                        ch_state=InvitationAccepted;
                    }
                    else if (ch_state==Synchronization)
                    {
                        manager->GetDataChannel()->PrepareSyncPacket(&tx_pkt_len);
                        manager->GetDataChannel()->SendPkt(tx_pkt_len);
                    }
                    
                }
            }
        }
    }
    

}

void AppleMIDIChannelsManager::StopConnection()
{
    scdf::ThreadUtils::TerminateThread(this->handle);
    controlChannel.Release();
    dataChannel.Release();
}

void AppleMIDIChannelsManager::StartConnection()
{
    int src_ctrl=50004;
    int dst_ctrl=5004;
    int src_stream=50005;
    int dst_stream=5005;
    
    char RemoteIp[40]="192.168.1.164";
    char LocalIp[40]="192.168.1.52";
    
    controlChannel.Init(LocalIp, RemoteIp, dst_ctrl, src_ctrl, &my_write_fds[0],&my_read_fds[0], "AugmentedAppleMidi Crtl Channel" ,0x6db86c19);
    dataChannel.Init(LocalIp, RemoteIp, dst_stream, src_stream, &my_write_fds[1],&my_read_fds[1], "AugmentedAppleMidi Stream Channel", 0x6db86c1a);
    FD_ZERO(&readset);
    maxfd=my_read_fds[active_read_fds-1]+1;
    active_read_fds=2;
    active_write_fds=2;
    
    this->handle=scdf::ThreadUtils::CreateThread((start_routine)ConnectionProcedure, this);
}
