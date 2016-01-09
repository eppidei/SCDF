#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "headers/ADE_typedefs.h"
#include "headers/ADE_Utils.h"
#include "AppleMIDI_CtrlCh.h"
#include <errno.h>
#include <signal.h>
#include <assert.h>



static volatile bool gStopNow;
static void HandleSigInt(int sigraised)
// A handler for SIGINT that causes us to break out of the
// main event loop when the user types ^C.  This has the
// effect of quitting the program.
{
    assert(sigraised == SIGINT);


    gStopNow = true;
}

int main()
{

APPLEMIDI_CH_T *p_AppleMidiCtrlChannel;
APPLEMIDI_CH_T *p_AppleMidiStreamChannel;
int my_read_fds[2];
int my_write_fds[2];
int active_read_fds=2;
int active_write_fds=2;
int maxfd;
int result;
fd_set readset;
fd_set writeset;
int i;
int sender_src_ctrl=50004;
int sender_dst_ctrl=5004;
int receiver_src_ctrl=sender_dst_ctrl;
int receiver_dst_ctrl=sender_src_ctrl;
int sender_src_stream=50005;
int sender_dst_stream=5005;
int receiver_src_stream=sender_dst_stream;
int receiver_dst_stream=sender_src_stream;
char RemoteIp[40]="192.168.1.164";
char LocalIp[40]="192.168.1.52";
int ctrl_tx_pkt_len;
int stream_tx_pkt_len;
ssize_t rrrr;
short ctrl_rx_command;
short ctrl_ch_state;
short stream_rx_command;
short stream_ch_state;
struct timeval  tv  ;
int ctrl_rx_token;
int stream_rx_token;
int sel_cnt=0;

/************* INIT CTRL CHANNEL *****************/
AppleMidiCh_Init(&p_AppleMidiCtrlChannel);
AppleMidiCh_ConfigUDPReceiver(p_AppleMidiCtrlChannel,LocalIp,RemoteIp, receiver_dst_ctrl, receiver_src_ctrl);
AppleMidiCh_ConfigUDPSender(p_AppleMidiCtrlChannel,LocalIp,RemoteIp, sender_dst_ctrl, sender_src_ctrl);
AppleMidiCh_GetSocketDescriptors(p_AppleMidiCtrlChannel,  &my_write_fds[0],&my_read_fds[0]);

AppleMidiCh_SetSenderSource(p_AppleMidiCtrlChannel,0x6db86c19);

AppleMidiCh_SetName(p_AppleMidiCtrlChannel,"AugmentedAppleMidi Crtl Channel");
/************* INIT STREAM CHANNEL *****************/
AppleMidiCh_Init(&p_AppleMidiStreamChannel);
AppleMidiCh_ConfigUDPReceiver(p_AppleMidiStreamChannel,LocalIp,RemoteIp, receiver_dst_stream, receiver_src_stream);
AppleMidiCh_ConfigUDPSender(p_AppleMidiStreamChannel,LocalIp,RemoteIp, sender_dst_stream, sender_src_stream);
AppleMidiCh_GetSocketDescriptors(p_AppleMidiStreamChannel,  &my_write_fds[1],&my_read_fds[1]);

AppleMidiCh_SetSenderSource(p_AppleMidiStreamChannel,0x6db86c1a);

AppleMidiCh_SetName(p_AppleMidiStreamChannel,"AugmentedAppleMidi Stream Channel");

signal(SIGINT,  HandleSigInt);


FD_ZERO(&readset);

//FD_ZERO(&writeset);
//for (i=0;i<active_write_fds;i++)
//{
//
//    FD_SET(my_write_fds[i], &writeset);
//    maxfd = (maxfd>my_write_fds[i])?maxfd:my_write_fds[i];
//}

//SADE_UdpReceiver_Recv(p_AppleMidiCtrlChannel->p_Receiver,&rrrr);
maxfd=my_read_fds[active_read_fds-1]+1;
while(!gStopNow)
{
//if (sel_cnt==1)
//{
//ADE_UdpReceiver_Recv(p_AppleMidiStreamChannel->p_Receiver,&rrrr);
//fprintf(stderr," RECEIVED\n");
//}
for (i=0;i<active_read_fds;i++)
{

    FD_SET(my_read_fds[i], &readset);
   // maxfd = (maxfd>my_read_fds[i])?maxfd:my_read_fds[i];
}
 tv  = {10,0}; /* SU LINUX TV VA SEMPRE REINIZIALIZZATO*/
result = select(maxfd+1, &readset,NULL , NULL, &tv);//&writeset
if (result == -1) {
   fprintf(stderr," SELECT ERROR ERRNO %d\n",errno);
}
else if (result == 0) {
   fprintf(stderr," SELECT TIMED OUT\n");
}
else {
sel_cnt++;
   for (i=0; i<active_read_fds; i++) {
      if (FD_ISSET(my_read_fds[i], &readset)) {
         if (i==0)
         {
            printf("CTRL CHANNELL RECEIVED\n");
            AppleMidiCh_ParsePacket(p_AppleMidiCtrlChannel);
            AppleMidiCh_GetReceivedCommand(p_AppleMidiCtrlChannel, &ctrl_rx_command);
            AppleMidiCh_GetReceivedToken(p_AppleMidiCtrlChannel, &ctrl_rx_token);
            AppleMidiCh_SetIniziatorToken(p_AppleMidiCtrlChannel,ctrl_rx_token);
            ctrl_ch_state=ctrl_rx_command;
            if (ctrl_ch_state==Invitation)
            {
             AppleMidiCh_PrepareAcceptInvitationPacket(p_AppleMidiCtrlChannel,&ctrl_tx_pkt_len);
             AppleMidiCh_SendPkt(p_AppleMidiCtrlChannel, ctrl_tx_pkt_len);
             ctrl_ch_state=InvitationAccepted;
            } else if (ctrl_ch_state==ReceiverFeedback)
            {
                printf("CTRL CHANNELL FEEDBACK\n");
            }
         }
         else if (i==1)
         {

            printf("STREAM CHANNELL RECEIVED\n");
            AppleMidiCh_ParsePacket(p_AppleMidiStreamChannel);
            AppleMidiCh_GetReceivedCommand(p_AppleMidiStreamChannel, &stream_rx_command);
            AppleMidiCh_GetReceivedToken(p_AppleMidiStreamChannel, &stream_rx_token);
            AppleMidiCh_SetIniziatorToken(p_AppleMidiStreamChannel,stream_rx_token);
            stream_ch_state=stream_rx_command;
            if (stream_ch_state==Invitation)
            {
             AppleMidiCh_PrepareAcceptInvitationPacket(p_AppleMidiStreamChannel,&stream_tx_pkt_len);
             AppleMidiCh_SendPkt(p_AppleMidiStreamChannel, stream_tx_pkt_len);
             stream_ch_state=InvitationAccepted;
            }
            else if (stream_ch_state==Synchronization)
            {
                AppleMidiCh_PrepareSyncPacket(p_AppleMidiStreamChannel,&stream_tx_pkt_len);
                AppleMidiCh_SendPkt(p_AppleMidiStreamChannel, stream_tx_pkt_len);
            }

         }
      }
   }


//   for (i=0; i<active_write_fds; i++) {
//      if (FD_ISSET(my_write_fds[i], &writeset)) {
//         if (i==0)
//         {
//            if (ctrl_ch_state==Invitation)
//            {
//             AppleMidiCh_PrepareAcceptInvitationPacket(p_AppleMidiCtrlChannel,&tx_pkt_len);
//             AppleMidiCh_SendCtrlPkt(p_AppleMidiCtrlChannel, tx_pkt_len);
//             ctrl_ch_state=InvitationAccepted;
//            }
//        }
//      }
//   }
}


} // end while


AppleMidiCh_Release(p_AppleMidiCtrlChannel);
AppleMidiCh_Release(p_AppleMidiStreamChannel);
return 0;
}
