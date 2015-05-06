#ifndef MIDI_CONNECTION_ANDROID_H_
#define MIDI_CONNECTION_ANDROID_H_


#include "MidiOutConnection.h"
#include "../usb/UsbMidiInterface.h"

//namespace Usb { class MidiInterface; }

namespace Scdf  {

    class MidiOutConnectionAndroid : public MidiOutConnection, Usb::MidiInterface::Listener
    {
    	friend MidiOutConnection* MidiOutConnection::Create(s_int32 index);
    	friend void MidiOutConnection::Destroy(MidiOutConnection* connection);

    	s_bool SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel);
        s_bool SendProgramChange(s_uint16 program, s_uint16 channel);
        s_bool SendAftertouch(s_uint16 value, s_uint16 channel);
        s_bool SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel);
        s_bool SendModWheel(s_uint16 value, s_uint16 channel);

        void OnUsbInterfaceDestroyed(Usb::AudioInterface* destroyedItf);

        void AttachListenerConnectionLost(MidiConnectionListener* _listener );
        void DetachListenerConnectionLost();

    private:

        MidiOutConnectionAndroid(s_int32 index);
        ~MidiOutConnectionAndroid();

        Usb::MidiInterface* itf;

    };
}












#endif
