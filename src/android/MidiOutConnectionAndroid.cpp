#include "MidiOutConnectionAndroid.h"
#include "UsbHandler.h"
#include "usb/UsbMidiInterface.h"

using namespace Scdf;


s_int32 MidiOutConnection::GetNumAvailableOutputs()
{
	return UsbHandler::GetNumMidiOutInterfaces();
}

std::string MidiOutConnection::GetOutputName(s_int32 index)
{
	return UsbHandler::GetMidiInterfaceName(index);
}

MidiOutConnection* MidiOutConnection::Create(s_int32 index)
{
	MidiOutConnection* connection = new MidiOutConnectionAndroid(index);
	//connection->itf->AddInterfaceEventListener(this);
	return connection;
}

void MidiOutConnection::Destroy(MidiOutConnection* connection)
{
	//connection->itf->RemoveInterfaceEventListener(this);
	delete connection;
}

MidiOutConnectionAndroid::MidiOutConnectionAndroid(s_int32 index)
{
	itf = UsbHandler::GetMidiInterface(index);
	if (itf!=NULL) {
		itf->DetachAndClaim();
		itf->AddListener(this);
	}
}


MidiOutConnectionAndroid::~MidiOutConnectionAndroid()
{
	if (itf!=NULL) {
		itf->RemoveListener(this);
	}
}

void MidiOutConnectionAndroid::OnUsbInterfaceDestroyed(Usb::AudioInterface* destroyedItf)
{
	if (destroyedItf==itf)
		itf = NULL;

	if (listener)
		listener->OnConnectionLost(this);
}

/*
 *  0x80     Note Off
   0x90     Note On
   0xA0     Aftertouch
   0xB0     Continuous controller
   0xC0     Patch change
   0xD0     Channel Pressure
   0xE0     Pitch bend
   0xF0     (non-musical commands)
 */


s_bool MidiOutConnectionAndroid::SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0x90) | channel;
	msg|=(note<<8);
	msg|=(velocity<<16);
	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0x80) | (channel&0xF);
	msg|=(note<<8);
	msg|=(velocity<<16);
	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0xB0) | (channel&0xF);
	msg|=(control<<8);
	msg|=(value<<16);

	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendProgramChange(s_uint16 program, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0xC0) | (channel&0xF);
	msg|=(program<<8);
	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendAftertouch(s_uint16 value, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0xA0) | (channel&0xF);
	msg|=(value<<16);
	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0xD0) | (channel&0xF);
	msg|=(note<<8);
	msg|=(value<<16);
	return itf->SendMIDIMessage(msg);
}

s_bool MidiOutConnectionAndroid::SendModWheel(s_uint16 value, s_uint16 channel)
{
	s_uint32 msg = 0;
	msg|=(0xD0) | (channel&0xF);
	msg|=(value << 8);
	//msg|=(value&0xFF00 << 8);
	return itf->SendMIDIMessage(msg);
}


