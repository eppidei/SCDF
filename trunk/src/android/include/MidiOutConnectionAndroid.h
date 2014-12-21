#ifndef MIDI_CONNECTION_ANDROID_H_
#define MIDI_CONNECTION_ANDROID_H_


#include "MidiOutConnection.h"

namespace Usb { class MidiInterface; }

namespace Scdf  {

    class MidiOutConnectionAndroid : public MidiOutConnection
    {
    	friend MidiOutConnection* MidiOutConnection::Create(s_int32 index);

    	s_bool SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel);
        s_bool SendProgramChange(s_uint16 program, s_uint16 channel);
        s_bool SendAftertouch(s_uint16 value, s_uint16 channel);
        s_bool SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel);
        s_bool SendModWheel(s_uint16 value, s_uint16 channel);

    private:

        void OnMidiInterfaceDisconnected(Usb::MidiInterface* itf);

        MidiOutConnectionAndroid(s_int32 index);
        Usb::MidiInterface* itf;

    };
}












#endif
