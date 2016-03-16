//
//  RTPPacket.cpp
//  SDCF_Framework_iOS
//
//  Created by andrea scuderi on 21/11/15.
//  Copyright © 2015 fuzone. All rights reserved.
//

#include <stdio.h>
#include "RTPPacket.h"
#include <sys/endian.h>
#define HEADER_SIZE 12
#define MIDI_PAYLOAD_HEADER_SIZE 1

#define HEADER_VERSION_SIZE 2
#define HEADER_PADDING_SIZE 1
#define HEADER_EXTENSION_SIZE 1
#define HEADER_CSRC_COUNT_SIZE 4
#define HEADER_MARKER_SIZE 1
#define HEADER_PAYLOAD_TYPE_SIZE 7
#define HEADER_SEQUENCE_NUMBER_SIZE 16
#define HEADER_TIMESTAMP_SIZE 32
#define HEADER_SSRC_SIZE 32 // synchronization source id

// optional:
// list of contributing source id
#define HEADER_CSRC_LIST_SIZE_MAX 15
#define HEADER_CSRC_SIZE 32

using namespace scdf;

RTPpacket::~RTPpacket()
{
    if (header)
        delete [] header;
    if (payload)
        delete [] payload;
}

void RTPpacket::BuildHeader()
{
    if (!header)
        header = new s_byte[HEADER_SIZE];
//    
//    int bitCounter=0;
//    
//    //version: This field identifies the version of RTP. The version defined by this specification is two (2). (The value 1 is used by the first draft version of RTP and the value 0 is used by the protocol initially implemented in the “vat” audio tool.)
//    for (int i=0;i<HEADER_VERSION_SIZE;++bitCounter, ++i)
//        header[bitCounter] = version&(1<<i);
//    
//    //padding: If the padding bit is set, the packet contains one or more additional padding octets at the end which are not part of the payload. The last octet of the padding contains a count of how many padding octets should be ignored, including itself. Padding may be needed by some encryption algorithms with fixed block sizes or for carrying several RTP packets in a lower-layer protocol data unit.
//    for (int i=0;i<HEADER_PADDING_SIZE;++bitCounter, ++i)
//        header[bitCounter] = padding&(1<<i);
//    
//    //extension: If the extension bit is set, the fixed header must be followed by exactly one header extension
//    for (int i=0;i<HEADER_EXTENSION_SIZE;++bitCounter, ++i)
//        header[bitCounter] = extension&(1<<i);
//    
//    //CSRC count 4 bit: The CSRC count contains the number of CSRC identifiers that follow the fixed header
//    for (int i=0;i<HEADER_CSRC_COUNT_SIZE;++bitCounter, ++i)
//        header[bitCounter] = cc&(1<<i);
//    
//    //marker: The interpretation of the marker is defined by a profile. It is intended to allow significant events such as frame boundaries to be marked in the packet stream. A profile may define additional marker bits or specify that there is no marker bit by changing the number of bits in the payload type field
//    for (int i=0;i<HEADER_MARKER_SIZE;++bitCounter, ++i)
//        header[bitCounter] = marker&(1<<i);
//    
//    //payload type: This field identifies the format of the RTP payload and determines its interpretation by the application. A profile may specify a default static mapping of payload type codes to payload formats. Additional payload type codes may be defined dynamically through non-RTP means. A set of default mappings for audio and video is specified in the companion RFC 3551. An RTP source may change the payload type during a session, but this field should not be used for multiplexing separate media streams. A receiver must ignore packets with payload types that it does not understand.
//    for (int i=0;i<HEADER_PAYLOAD_TYPE_SIZE;++bitCounter, ++i)
//        header[bitCounter] = payloadType&(1<<i);
//    
//    //sequence number: The sequence number increments by one for each RTP data packet sent, and may be used by the receiver to detect packet loss and to restore packet sequence. The initial value of the sequence number should be random (unpredictable) to make known-plaintext attacks on encryption more difficult, even if the source itself does not encrypt according to the method in Section 9.1, because the packets may flow through a translator that does
//    for (int i=0;i<HEADER_SEQUENCE_NUMBER_SIZE;++bitCounter, ++i)
//        header[bitCounter] = sequenceNumber&(1<<i);
//    
//    //timestamp: The timestamp reflects the sampling instant of the first octet in the RTP data packet. The sampling instant must be derived from a clock that increments monotonically and linearly in time to allow synchronization and jitter calculations (see Section 6.4.1). The resolution of the clock must be sufficient for the desired synchronization accuracy and for measuring packet arrival jitter (one tick per video frame is typically not sufficient). The clock frequency is dependent on the format of data carried as payload and is specified statically in the profile or payload format specification that defines the format, or may be specified dynamically for payload formats defined through non-RTP means. If RTP packets are generated periodically, the nominal sampling instant as determined from the sampling clock is to be used, not a reading of the system clock. As an example, for fixed-rate audio the timestamp clock would likely increment by one for each sampling period. If an audio application reads blocks covering 160 sampling periods from the input device, the timestamp would be increased by 160 for each such block, regardless of whether the block is transmitted in a packet or dropped as silent. The initial value of the timestamp should be random, as for the sequence number.
//    for (int i=0;i<HEADER_TIMESTAMP_SIZE;++bitCounter, ++i)
//        header[bitCounter] = timeStamp&(1<<i);
//    
//    //SSRC: The SSRC field identifies the synchronization source. This identifier should be chosen randomly, with the intent that no two synchronization sources within the same RTP session will have the same SSRC identifier. An example algorithm for generating a random identifier is presented in Appendix A.6. Although the probability of multiple sources choosing the same identifier is low, all RTP implementations must be prepared to detect and resolve collisions. Section 8 describes the probability of collision along with a mechanism for resolving collisions and detecting RTP-level forwarding loops based on the uniqueness of the SSRC identifier. If a source changes its source transport address, it must also choose a new SSRC identifier to avoid being interpreted as a looped source.
//    for (int i=0;i<HEADER_SSRC_SIZE;++bitCounter, ++i)
//        header[bitCounter] = ssrc&(1<<i);
//    
//    //CSRC list: 0 to 15 items, 32 bits each: The CSRC list identifies the contributing sources for the payload contained in this packet. The number of identifiers is given by the CC field. If there are more than 15 contributing sources, only 15 can be identified. CSRC identifiers are inserted by mixers (see Section 7.1), using the SSRC identifiers of contributing sources. For example, for audio packets the SSRC identifiers of all sources that were mixed together to create a packet are listed, allowing correct talker indication at the receiver.
//    
    header[0]=128;
    header[1]=97;
    short sequence=sequenceNumber;
    sequence=htons(sequence);
    timeStamp=htonl(timeStamp);
    ssrc=htonl(ssrc);
    memcpy(&header[2], &sequence, sizeof(short));
    memcpy(&header[4], &timeStamp, sizeof(s_int32));
    memcpy(&header[8], &ssrc, sizeof(s_int32));
}

void RTPpacket::Create(s_int32 _payloadType, s_int32 packetId, s_int32 time, s_byte *data, s_int32 data_length)
{
    header=NULL;
    payload=NULL;
    
    //fill by default header fields:
    version = 2;
    padding = 0;
    extension = 0;
    cc = 0;
    marker = 0;
    static s_int32 pippo=rand();
    ssrc = pippo;
    sequenceNumber = packetId;
    timeStamp = time;
    payloadType = _payloadType;
    
    BuildHeader();
    AttachPayload(data, data_length);

}

void RTPpacket::AttachPayload(s_byte *data, s_int32 data_length)
{
    payload_size = data_length;
    payload = new s_byte[payload_size];
    memcpy(payload, data, data_length);
}

void RTPMIDIPacket::AttachPayload(s_byte *data, s_int32 data_length)
{
    payload_size = data_length+MIDI_PAYLOAD_HEADER_SIZE;
    payload = new s_byte[payload_size];
    payload[0]=0; //MIDI list lenght size = 8 bit (a value = 1 means MIDI list lenght size = 12 bit)
    payload[0]=0; //Journal extension presence
    payload[0]=0; //First delta time presence
    payload[0]=0; //Status byte presence on the first MIDI channel command
    //MIDI list lenght
    s_byte numData=data_length;//((2*data_length)-1)<<4;
    payload[0]=data_length;
//    payload[0]=data_length&(1<<0);
//    payload[0]=data_length&(1<<1);
//    payload[0]=data_length&(1<<2);
//    payload[0]=data_length&(1<<3);
    
    memcpy(payload+MIDI_PAYLOAD_HEADER_SIZE, data, data_length);
}

int RTPpacket::GetPacket(s_byte **_packet)
{
    //construct the packet = header + payload
    s_byte *packet=*_packet;
    (*_packet)= new s_byte[payload_size + HEADER_SIZE];
    for (int i=0; i < HEADER_SIZE; i++)
        (*_packet)[i] = header[i];
    for (int i=0; i < payload_size; i++)
        (*_packet)[i+HEADER_SIZE] = payload[i];
    
    //return total size of the packet
    return(payload_size + HEADER_SIZE);
}
