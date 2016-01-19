//
//  FuzAppleMIDIChannel.hpp
//  RtpMIDISession
//
//  Created by andrea scuderi on 09/01/16.
//  Copyright © 2016 fuzone. All rights reserved.
//

#ifndef FuzAppleMIDIChannel_hpp
#define FuzAppleMIDIChannel_hpp

#include <stdio.h>
#include "FuzAppleMIDIDefines.h"
#include "ThreadUtils.h"

class AppleMIDIChannelsManager;
class AppleMIDIChannel
{
    friend class AppleMIDIChannelsManager;
private:
    APPLEMIDI_CH_T *channel;

    int DoInit();
    void ConfigUDPReceiverProbe(int DstPort);
    void ConfigUDPSender(char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort);
    void ProbeStreamInfo();
    void ConfigUDPReceiver(char *p_LocalIp,char *p_RemoteIp, int DstPort, int SrcPort);
    void PrepareAcceptInvitationPacket(int *p_actual_len);
    void ParsePacket();
    void GetSocketDescriptors(int *p_TxDesc, int *p_RxDesc);
    void SetSenderSource(unsigned int SenderSrc);
    void SetIniziatorToken(int Token);
    void SetName(const char *p_name);
    void SendPkt(size_t BuffSize);
    void GetReceivedCommand(short *p_Command);
    void GetReceivedSyncCount(short *p_count);
    void Release();
    void GetReceivedToken(int *p_Token);
    void PrepareSyncPacket(int *p_actual_len);
public:
    void Init(char *p_LocalIp, char *p_RemoteIp, int DstPort, int SrcPort, int *p_TxDesc, int *p_RxDesc, std::string p_ChName, unsigned int SenderSrc);
    void InitProbeChannelOnly( int DstPort,int *remote_port,char *remote_ipv4name,size_t remote_name_size,int *local_port,char *local_ipv4name,size_t local_name_size);

};

class AppleMIDIChannelsManager
{
    AppleMIDIChannel controlChannel, dataChannel,probeChannel;
    int my_read_fds[2];
    int my_write_fds[2];
    int maxfd;
    int active_read_fds;
    int active_write_fds;
    fd_set readset;
    fd_set writeset;

    scdf::ThreadUtils::ThreadHandle handle;

    static void ConnectionProcedure(void *param);
    void InitConnection();
public:
    void StartConnection();
    void StopConnection();
    AppleMIDIChannel *GetControlChannel() {return &controlChannel;}
    AppleMIDIChannel *GetDataChannel() {return &dataChannel;}
};

#endif /* FuzAppleMIDIChannel_hpp */
