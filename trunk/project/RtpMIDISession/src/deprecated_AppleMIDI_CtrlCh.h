#ifndef _APPLEMIDI_CTRLCH
#define _APPLEMIDI_CTRLCH
#include "FuzAppleMIDIDefines.h"


typedef struct APPLEMIDI_CH_S APPLEMIDI_CH_T;
#ifdef __cplusplus
    extern "C" {
#endif
int AppleMidiCh_Init(APPLEMIDI_CH_T **dp_ch);
void AppleMidiCh_ConfigUDPSender(APPLEMIDI_CH_T *p_ch,char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort);
void AppleMidiCh_ConfigUDPReceiver(APPLEMIDI_CH_T *p_ch,char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort);
void AppleMidiCh_PrepareAcceptInvitationPacket(APPLEMIDI_CH_T *p_ch,int *p_actual_len);
void AppleMidiCh_ParsePacket(APPLEMIDI_CH_T *p_ch);
void AppleMidiCh_GetSocketDescriptors(APPLEMIDI_CH_T *p_ch, int *p_TxDesc, int *p_RxDesc);
void AppleMidiCh_SetSenderSource(APPLEMIDI_CH_T *p_ch,unsigned int SenderSrc);
void AppleMidiCh_SetIniziatorToken(APPLEMIDI_CH_T *p_ch,int Token);
void AppleMidiCh_SetName(APPLEMIDI_CH_T *p_ch,char *p_name);
void AppleMidiCh_SendPkt(APPLEMIDI_CH_T *p_ch, size_t BuffSize);
void AppleMidiCh_GetReceivedCommand(APPLEMIDI_CH_T *p_ch, short *p_Command);
void AppleMidiCh_Release(APPLEMIDI_CH_T *p_ch);
void AppleMidiCh_GetReceivedToken(APPLEMIDI_CH_T *p_ch, int *p_Token);
void AppleMidiCh_PrepareSyncPacket(APPLEMIDI_CH_T *p_ch,int *p_actual_len);
#ifdef __cplusplus
    }   /* extern "C" */
#endif
#endif //_ADE_APPLEMIDI_CTRLCH
