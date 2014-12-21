/*
 * ControlUnit.cpp
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#include "ControlUnit.h"
#include <sstream>

using namespace ScdfCtrl;

static int controlCounter = 0;

ControlUnit::ControlUnit()
{
	midiConnection = NULL;
	// init osc sender

	oscIp = "127.0.0.1";
	oscPort = 5555;
	oscTag = "";
	oscEnabled = false;

	midiMsgType = Invalid;
	midiChannel = -1;
	midiControl = -1;
    if (Scdf::MidiOutConnection::GetNumAvailableOutputs()!=0)
        SetMidiOutIndex(0);
    //#ifdef _DEBUG
    midiMsgType = NoteOn;
    midiChannel = 0;
    midiControl = ++controlCounter%127;
    //#endif
	lastOpenedMidiOutIndex = -1;
}

std::string ControlUnit::GetOscIp()
{
	return oscIp;
}

s_int32 ControlUnit::GetOscPort()
{
	return oscPort;
}

std::string ControlUnit::GetOscTag()
{
	if (!oscTag.empty()) return oscTag;

	std::stringstream ss;
	ss << "/";
	switch (midiMsgType)
	{
		case NoteOn: ss<<"note[on|off]/"; break;
		case ControlChange: ss<<"control/"; break;
		case ProgramChange: ss<<"program/"; break;
		case PitchBend: ss<<"pitchbend/"; break;
		case Aftertouch: ss<<"aftertouch/"; break;
		case PolyKeyPressure: ss<<"polypressure/"; break;
		default: return "[invalid]";
	}
	if (-1==midiChannel) return "[no channel]";
	ss << midiChannel << "/";
	return ss.str();
}

s_bool ControlUnit::IsOscEnabled()
{
	return oscEnabled;
}

s_int32 ControlUnit::GetMidiOutIndex()
{
	if (NULL==midiConnection) return -1;
	return lastOpenedMidiOutIndex; //midiConnection->GetOutIndex();
}

MidiMessageType ControlUnit::GetMidiMessageType()
{
	return midiMsgType;
}

s_int32 ControlUnit::GetMidiChannel()
{
	return midiChannel;
}

s_int32 ControlUnit::GetMidiControl()
{
	return midiControl;
}


void ControlUnit::SetOscIp(std::string ip)
{
	oscIp = ip;
}

void ControlUnit::SetOscPort(s_int32 port)
{
	oscPort = port;
}

void ControlUnit::SetOscEnabled(s_bool enabled)
{
	oscEnabled = enabled;
}

s_bool ControlUnit::SetMidiOutIndex(s_int32 index)
{
	if (midiConnection)
		Scdf::MidiOutConnection::Destroy(midiConnection);

	if (index == -1) {
		lastOpenedMidiOutIndex = -1;
		return true;
	}

	midiConnection = Scdf::MidiOutConnection::Create(index);
	midiConnection->SetListener(this);

	lastOpenedMidiOutIndex = midiConnection ? index : -1;

	return midiConnection != NULL;
}

void ControlUnit::SetMidiMessageType(MidiMessageType type)
{
	midiMsgType = type;
}

void ControlUnit::SetMidiChannel(s_int32 chan)
{
	midiChannel = chan;
}

void ControlUnit::SetMidiControl(s_int32 ctrl)
{
	midiControl = ctrl;
}

s_bool ControlUnit::SendValue(s_int32 number, s_int32 value)
{
	SetMidiControl(number);
	return SendValue(value);
}

s_bool ControlUnit::SendValue(s_int32 value)
{
	if (oscEnabled) {
		// send via osc
	}

	if (!midiConnection)
		return false;

	switch (midiMsgType) {
		case NoteOn:		return midiConnection->SendNoteOn(GetMidiPitch(),value,midiChannel);
		case NoteOff:		return midiConnection->SendNoteOff(GetMidiPitch(),value,midiChannel);
		case ControlChange: return midiConnection->SendControlChange(GetMidiControl(),value,midiChannel);
		case ProgramChange: return midiConnection->SendProgramChange(value,midiChannel);
		case PitchBend: 	return midiConnection->SendModWheel(value,midiChannel);
		case Aftertouch: 	return midiConnection->SendAftertouch(value,midiChannel);
		case PolyKeyPressure: return midiConnection->SendPolyKeyPressure(GetMidiPitch(),value,midiChannel);
		default: return false;
	}
	return false;

}

void ControlUnit::OnConnectionLost(Scdf::MidiOutConnection* connection)
{
	if (connection==midiConnection) {
		Scdf::MidiOutConnection::Destroy(midiConnection);
		midiConnection = NULL;
		lastOpenedMidiOutIndex = -1;
	}
}

