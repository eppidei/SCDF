//#include "AppleMIDI_CtrlCh.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>


//static AppleMidiCh_doParsePacket(char *p_buff, APPLEMIDI_T* p_AppleMIDIPkt);
//static void AppleMidiCh_doPrepareAcceptInvitationPacket(APPLEMIDI_T *p_pkt_tx,int IniziatorToken,unsigned int  SenderSource,char *p_name,int *p_actual_len);
//static ADE_UINT64_T htonll(ADE_UINT64_T val);
//static ADE_UINT64_T ntohll(ADE_UINT64_T val);
//static void AppleMidiCh_SetTimestamp(APPLEMIDI_CH_T *p_ch);
//static void AppleMidiCh_doPrepareSyncPacket(APPLEMIDI_T *p_pkt_tx,APPLEMIDI_T *p_pkt_rx,unsigned int SenderSource,ADE_UINT64_T tstamp,char count,int padding,int *p_actual_len);

//int AppleMidiCh_Init(APPLEMIDI_CH_T **dp_ch)
//{
//    APPLEMIDI_CH_T *p_this=NULL;
//
//    p_this=calloc(1,sizeof(APPLEMIDI_CH_T));
//
//    if (p_this!=NULL)
//    {
//        ADE_UdpSender_Init(&(p_this->p_Sender));
//        ADE_UdpReceiver_Init(&(p_this->p_Receiver));
//        *dp_ch=p_this;
//
//        return 0;
//    }
//    else
//    {
//
//        return -1;
//    }
//
//}
//void AppleMidiCh_ConfigUDPSender(APPLEMIDI_CH_T *p_ch,char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort)
//{
////ADE_UdpSender_CreateSocket(p_ch->p_Sender);
////ADE_UdpSender_SetLocal(p_ch->p_Sender,p_LocalIp,SrcPort);
////if (ADE_UdpSender_SetReusePort(p_ch->p_Sender)==-1)
////{
////ADE_UdpSender_SetReuseAddress(p_ch->p_Sender);
////}
///* TO DO : ADD some sanity checks */
//ADE_UdpSender_CloneSocket(p_ch->p_Sender,p_ch->p_Receiver);
//
//
//}
//void AppleMidiCh_ConfigUDPReceiver(APPLEMIDI_CH_T *p_ch,char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort)
//{
//ADE_UdpReceiver_CreateSocket(p_ch->p_Receiver);
//ADE_UdpReceiver_SetReuseAddress(p_ch->p_Receiver);
////if (ADE_UdpReceiver_SetReusePort(p_ch->p_Receiver)==-1)
////{
////
////}
//ADE_UdpReceiver_SetLocal(p_ch->p_Receiver,p_LocalIp,DstPort);
//ADE_UdpReceiver_SetRemote(p_ch->p_Receiver,p_RemoteIp,SrcPort);
////ADE_UdpReceiver_SetNonBlocking(p_ch->p_Receiver);
//}
//
//void AppleMidiCh_PrepareAcceptInvitationPacket(APPLEMIDI_CH_T *p_ch,int *p_actual_len)
//{
//
//AppleMidiCh_doPrepareAcceptInvitationPacket(&(p_ch->AppleMidiTx),p_ch->IniziatorToken,p_ch->SenderSource,p_ch->name,p_actual_len);
//
//
//}
//
//static void AppleMidiCh_doPrepareAcceptInvitationPacket(APPLEMIDI_T *p_pkt_tx,int IniziatorToken,unsigned int  SenderSource,char *p_name,int *p_actual_len)
//{
//
//
//    p_pkt_tx->header.Signature=htons(0xffff);
//    p_pkt_tx->header.Command=htons(InvitationAccepted);
//     p_pkt_tx->Invitation.ProtVersion=htonl(2);
//     p_pkt_tx->Invitation.IniziatorToken=htonl(IniziatorToken);
//     p_pkt_tx->Invitation.SenderSource=htonl(SenderSource);
//     strcpy(p_pkt_tx->Invitation.name,p_name);
//     p_pkt_tx->Invitation.name_length=strlen(p_name);
//
//
//     *p_actual_len=4+4+4+4+p_pkt_tx->Invitation.name_length+1;
//
//
//
//}

//static void AppleMidiCh_SetTimestamp(APPLEMIDI_CH_T *p_ch)
//{
//
//    struct timespec monotime;
//    clock_gettime(CLOCK_MONOTONIC, &monotime);
//    p_ch->Timestamp=monotime.tv_nsec;
//}
//
//void AppleMidiCh_PrepareSyncPacket(APPLEMIDI_CH_T *p_ch,int *p_actual_len)
//{
//    int padding = 0x0000;
//    AppleMidiCh_SetTimestamp(p_ch);
//    AppleMidiCh_doPrepareSyncPacket(&(p_ch->AppleMidiTx),&(p_ch->AppleMidiRx),p_ch->SenderSource,p_ch->Timestamp,p_ch->AppleMidiRx.Synchronization.Count,padding,p_actual_len);
//}
//
//static void AppleMidiCh_doPrepareSyncPacket(APPLEMIDI_T *p_pkt_tx,APPLEMIDI_T *p_pkt_rx,unsigned int SenderSource,ADE_UINT64_T tstamp,char count,int padding,int *p_actual_len)
//{
//int temp=( count << 24 | (0x00FFFFFF & padding) );
//
//    p_pkt_tx->header.Signature=htons(0xffff);
//    p_pkt_tx->header.Command=htons(Synchronization);
//    p_pkt_tx->Synchronization.SenderSSRC=htonl(SenderSource);
//   // p_pkt_tx->Synchronization.Count=count;//htonl(IniziatorToken);
//   // p_pkt_tx->Synchronization.Padding=0x000;//htonl(SenderSource);
//     p_pkt_tx->Synchronization.word=htonl(temp);
//    if (count==0)
//    {
//        p_pkt_tx->Synchronization.TimeStamp1= p_pkt_rx->Synchronization.TimeStamp1;
//        p_pkt_tx->Synchronization.TimeStamp2=tstamp;
//        p_pkt_tx->Synchronization.TimeStamp3=p_pkt_rx->Synchronization.TimeStamp3;
//    }
//    else if (count==1)
//    {
//        p_pkt_tx->Synchronization.TimeStamp1= p_pkt_rx->Synchronization.TimeStamp1;
//        p_pkt_tx->Synchronization.TimeStamp2=p_pkt_rx->Synchronization.TimeStamp2;
//        p_pkt_tx->Synchronization.TimeStamp3=tstamp;
//    }
//    else if (count==2)
//    {
//         p_pkt_tx->Synchronization.TimeStamp1= tstamp;
//        p_pkt_tx->Synchronization.TimeStamp2=p_pkt_rx->Synchronization.TimeStamp2;
//        p_pkt_tx->Synchronization.TimeStamp3=p_pkt_rx->Synchronization.TimeStamp3;
//    }
//
//     *p_actual_len=4+4+4+8+8+8;
//
//}
//void AppleMidiCh_ParsePacket(APPLEMIDI_CH_T *p_ch)
//{
//    ssize_t n_recv_bytes;
//
//    ADE_UdpReceiver_Recv(p_ch->p_Receiver,&n_recv_bytes);
//    AppleMidiCh_doParsePacket(p_ch->p_Receiver->RecvBuff.p_Buff,&(p_ch->AppleMidiRx));
//
//}

//void AppleMidiCh_GetReceivedCommand(APPLEMIDI_CH_T *p_ch, short *p_Command)
//{
//
//*p_Command=p_ch->AppleMidiRx.header.Command;
//
//}

//void AppleMidiCh_GetReceivedToken(APPLEMIDI_CH_T *p_ch, int *p_Token)
//{
//
//*p_Token=p_ch->AppleMidiRx.Invitation.IniziatorToken;
//
//}
//
//static  AppleMidiCh_doParsePacket(char *p_buff, APPLEMIDI_T* p_AppleMIDIPkt)
//{
//
//    unsigned int offset = 0;
//    APPLEMIDI_T * p_int_buff=NULL;
//
//    p_int_buff=(APPLEMIDI_T *)p_buff;
//    short Command,Signature;
//
//    Command=ntohs(p_int_buff->header.Command);
//     Signature=ntohs(p_int_buff->header.Signature);
//
//     p_AppleMIDIPkt->header.Command=Command;
//     p_AppleMIDIPkt->header.Signature=Signature;
//
//
//    if (Command==Invitation || Command==InvitationRejected || Command==InvitationAccepted )
//    {
//
//        p_AppleMIDIPkt->Invitation.ProtVersion=ntohl(p_int_buff->Invitation.ProtVersion);
//        p_AppleMIDIPkt->Invitation.IniziatorToken=ntohl(p_int_buff->Invitation.IniziatorToken);
//        p_AppleMIDIPkt->Invitation.SenderSource=ntohl(p_int_buff->Invitation.SenderSource);
//        strcpy(p_AppleMIDIPkt->Invitation.name,p_int_buff->Invitation.name);
//
//    }
//    else if (Command==Synchronization)
//    {
//        p_AppleMIDIPkt->Synchronization.SenderSSRC=ntohl(p_int_buff->Synchronization.SenderSSRC);
//        p_AppleMIDIPkt->Synchronization.Count=ntohl(p_int_buff->Synchronization.Count);
//        p_AppleMIDIPkt->Synchronization.Padding=ntohl(p_int_buff->Synchronization.Padding);
//        p_AppleMIDIPkt->Synchronization.TimeStamp1=ntohll(p_int_buff->Synchronization.TimeStamp1);
//        p_AppleMIDIPkt->Synchronization.TimeStamp2=ntohll(p_int_buff->Synchronization.TimeStamp2);
//        p_AppleMIDIPkt->Synchronization.TimeStamp3=ntohll(p_int_buff->Synchronization.TimeStamp3);
//
//    }
//    else if (Command==ReceiverFeedback)
//    {
//        p_AppleMIDIPkt->RecvFeedBack.SenderSSRC=ntohl(p_int_buff->RecvFeedBack.SenderSSRC);
//        p_AppleMIDIPkt->RecvFeedBack.SequenceNumber=ntohl(p_int_buff->RecvFeedBack.SequenceNumber);
//
//    }
//    else
//    {
//
//        fprintf(stderr, "APPLE COMMAND UNKNOWN !\n");
//
//    }
//

//}

//void AppleMidiCh_GetSocketDescriptors(APPLEMIDI_CH_T *p_ch, int *p_TxDesc, int *p_RxDesc)
//{
//
//ADE_UdpSender_GetDescriptor(p_ch->p_Sender,p_TxDesc);
//ADE_UdpReceiver_GetDescriptor(p_ch->p_Receiver,p_RxDesc);
//
//
//}

//void AppleMidiCh_SetSenderSource(APPLEMIDI_CH_T *p_ch,unsigned int SenderSrc)
//{
//
//p_ch->SenderSource=SenderSrc;
//}

//void AppleMidiCh_SetIniziatorToken(APPLEMIDI_CH_T *p_ch,int Token)
//{
//
//p_ch->IniziatorToken=Token;
//}


//void AppleMidiCh_SetName(APPLEMIDI_CH_T *p_ch,char *p_name)
//{
//
//strcpy(p_ch->name,p_name);
//}


//void AppleMidiCh_SendPkt(APPLEMIDI_CH_T *p_ch, size_t BuffSize)
//{
//
//ssize_t SentSize;
//
//ADE_UdpSender_SetExternallBuff(p_ch->p_Sender,&(p_ch->AppleMidiTx), BuffSize);
//ADE_UdpSender_SendExternalBuff(p_ch->p_Sender,BuffSize, &SentSize);
//
//}

//static ADE_UINT64_T htonll(ADE_UINT64_T val)
//{
//    union {ADE_UINT32_T lw[2];ADE_UINT64_T llw;} u;
//    u.lw[0]=htonl(val>>32);
//    u.lw[1]=htonl(val & 0xFFFFFFFFULL);
//    return u.llw;
//}
//
//static ADE_UINT64_T ntohll(ADE_UINT64_T val)
//{
//    union {ADE_UINT32_T lw[2];ADE_UINT64_T llw;} u;
//    u.llw = val;
//
//    return  ( (ADE_UINT64_T) ntohl(u.lw[0])<<32) | (ADE_UINT64_T)ntohl (u.lw[1]);
//}
//
//void AppleMidiCh_Release(APPLEMIDI_CH_T *p_ch)
//{
//    ADE_UdpSender_Release(p_ch->p_Sender);
//    ADE_UdpReceiver_Release(p_ch->p_Receiver);
//    free(p_ch);
//
//}
