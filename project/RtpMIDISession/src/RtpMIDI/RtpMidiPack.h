#pragma once

#include "RtpMidi_Util.h"
#include <sstream>

#define PAYLOADTYPE_RTPMIDI 97

class RtpMidiPack {
    std::ostringstream os;
public:
	uint8_t		ddddd;
	uint8_t		playLoadType;
	uint16_t	sequenceNr;
	uint32_t	timestamp;
	uint32_t	ssrc;

    char *GetPacket()
    {
        return const_cast<char*>(os.str().c_str());
    }

    int GetPacketSize()
    {
        return os.str().size();
    }

	RtpMidiPack()
	{
		ddddd = 0b10000000; // TODO
		playLoadType = PAYLOADTYPE_RTPMIDI;
	}

	void beginWrite()
	{
		_writeHeader();
	}
	void endWrite()
	{
	}
    void write(void *p_value, int size)
    {
        char *ptr=(char*)p_value;
        for (int i=0;i<size/sizeof(char);++i)
        {
            os<<ptr[i];
        }
    }
private:
	void _writeHeader()
	{
        os.str("");
        os.clear();
        os<<ddddd;
        os<<playLoadType;

		uint16_t _sequenceNr = MIDI_Util::toEndian(sequenceNr);
		uint32_t _timestamp  = MIDI_Util::toEndian(timestamp);
		uint32_t _ssrc       = MIDI_Util::toEndian(ssrc);

        write(&_sequenceNr, sizeof(_sequenceNr));
        write(&_timestamp, sizeof(_timestamp));
        write(&_ssrc, sizeof(_ssrc));

	}

};


