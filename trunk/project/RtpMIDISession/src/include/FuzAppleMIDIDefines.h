#ifndef _APPLE_MIDI_H
#define _APPLE_MIDI_H

#include "headers/ADE_typedefs.h"
#include "headers/ADE_Udp_Sender.h"
#include "headers/ADE_Udp_Receiver.h"

/****************** ADE APPLE MIDI ********************/
typedef enum
{
    EndSession = 0x4259, /*   "BY"   */
    Synchronization = 0x434b, /*   "CK"   */
    Invitation = 0x494e, /*   "IN"   */
    InvitationRejected = 0x4e4f, /*   "NO"   */
    InvitationAccepted = 0x4f4b, /*   "OK"   */
    BitRateReceiveLimit = 0x524c, /*   "RL"   */
    ReceiverFeedback = 0x5253
} APPLEMIDI_COMMAND_T;

typedef struct APPLE_MIDI_HEADER_S
{
    short Signature;
    short Command;
} APPLE_MIDI_HEADER_T;

typedef struct APPLE_MIDI_INVITATION_PCKT_S
{
    int ProtVersion;
    int IniziatorToken;
    unsigned int SenderSource;
    char name[256];
    int name_length;
} APPLE_MIDI_INVITATION_PCKT_T;

typedef struct APPLE_MIDI_SYNC_PCKT_S
{
    int SenderSSRC;
    union
    {
        int Count:8;
        int Padding :24;
        int word;
    };
    ADE_INT64_T TimeStamp1;
    ADE_INT64_T TimeStamp2;
    ADE_INT64_T TimeStamp3;
} APPLE_MIDI_SYNC_PCKT_T;

typedef struct APPLE_MIDI_RECVFB_PCKT_S
{
    int SenderSSRC;
    ADE_INT64_T SequenceNumber;
} APPLE_MIDI_RECVFB_PCKT_T;

typedef struct APPLEMIDI_S
{
    APPLE_MIDI_HEADER_T header;
    union
    {
        APPLE_MIDI_RECVFB_PCKT_T RecvFeedBack;
        APPLE_MIDI_SYNC_PCKT_T Synchronization;
        APPLE_MIDI_INVITATION_PCKT_T Invitation;
    };
} __attribute__ ((__packed__))  APPLEMIDI_T;

struct APPLEMIDI_CH_S
{
    ADE_UDPSENDER_T *p_Sender;
    ADE_UDPRECEIVER_T *p_Receiver;
    APPLEMIDI_T AppleMidiRx;
    APPLEMIDI_T AppleMidiTx;
    int IniziatorToken;
    unsigned int SenderSource;
    ADE_INT64_T Timestamp;
    char name[40];
};

typedef APPLEMIDI_CH_S APPLEMIDI_CH_T;
#endif //_ADE_APPLE_MIDI_H
