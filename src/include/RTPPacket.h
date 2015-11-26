//
//  RTPPacket.h
//  SDCF_Framework_iOS
//
//  Created by andrea scuderi on 21/11/15.
//  Copyright © 2015 fuzone. All rights reserved.
//

#ifndef RTPPacket_h
#define RTPPacket_h

#include "TypeDefinitions.h"

namespace scdf
{
    class RTPpacket
    {
        
    public:
        void Create(s_int32 paylodType, s_int32 packetId, s_int32 time, s_byte *data, s_int32 data_length);
        ~RTPpacket();
        
        int GetPacket(s_byte **packet);
        
    protected:
        //internal fields
        s_byte *header, *payload;
        s_int32 payload_size;
        
        virtual void AttachPayload(s_byte *data, s_int32 data_length);
    private:
        //fields that compose the RTP header
        s_int32 version;
        s_int32 padding;
        s_int32 extension;
        s_int32 cc;
        s_int32 marker;
        s_int32 payloadType;
        s_int32 sequenceNumber;
        s_int32 timeStamp;
        s_int32 ssrc;
        
        void BuildHeader();
    };
    
    class RTPMIDIPacket : public RTPpacket
    {
    protected:
        void AttachPayload(s_byte *data, s_int32 data_length) override;
    };
}

#endif /* RTPPacket_h */
