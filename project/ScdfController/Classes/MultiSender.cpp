/*
 * MultiSender.cpp
 *
 *  Created on: 21 Feb 2015
 *      Author: athos
 */





#include "MultiSender.h"
#include "RTPPacket.h"
#include <sstream>

using namespace ScdfCtrl;

static int controlCounter = 0;

MultiSender::MultiSender()
{
	midiConnection = NULL;
	// init osc sender

	//oscIp = "127.0.0.1";
	//oscPort = 5555;
	udpSender.InitEndpoints(5555,1,"192.168.1.100");
	oscTag = "";
	oscEnabled = false;

//	midiMsgType = Invalid;
//	midiChannel = -1;
//	midiControl = -1;
////    if (Scdf::MidiOutConnection::GetNumAvailableOutputs()!=0)
////        SetMidiOutIndex(0);
//    //#ifdef _DEBUG
    midiMsgType = NoteOn;
    midiChannel = 0;
    midiControl = 0;
    //#endif
	lastOpenedMidiOutIndex = -1;
}

std::string MultiSender::GetOscIp()
{
	return udpSender.GetAddress();
}

s_int32 MultiSender::GetOscPort()
{
	return udpSender.GetPort();
}

std::string MultiSender::GetOscTag()
{
	if (!oscTag.empty()) return oscTag;

	std::stringstream ss;
	ss << "/";
	switch (midiMsgType)
	{
		case NoteOn: ss<<"noteon/"; break;
		case ControlChange: ss<<"control/"; break;
		case ProgramChange: ss<<"program/"; break;
		case PitchBend: ss<<"pitchbend/"; break;
		case Aftertouch: ss<<"aftertouch/"; break;
		case PolyKeyPressure: ss<<"polypressure/"; break;
		default: return "[invalid]";
	}
    if (midiChannel<-1) return "[no channel]";
    else if (midiChannel==-1) ss << "Any" << "/";
    else ss << midiChannel << "/";
	return ss.str();
}

s_bool MultiSender::IsOscEnabled()
{
	return oscEnabled;
}

s_int32 MultiSender::GetMidiOutIndex()
{
	if (NULL==midiConnection) return -1;
	return lastOpenedMidiOutIndex; //midiConnection->GetOutIndex();
}

MidiMessageType MultiSender::GetMidiMessageType()
{
	return midiMsgType;
}

s_int32 MultiSender::GetMidiChannel()
{
	return midiChannel;
}

s_int32 MultiSender::GetMidiControl()
{
	return midiControl;
}


void MultiSender::SetOscIp(std::string ip)
{
	udpSender.SetAddress(ip);
}

void MultiSender::SetOscPort(s_int32 port)
{
	udpSender.SetPort(port);
}

void MultiSender::SetOscEnabled(s_bool enabled)
{
	oscEnabled = enabled;
}

osc::OutboundPacketStream MultiSender::PackOSCValue(s_int32 ctrl, s_int32 value, std::string tag)
{
	s_char oscBuff[1000]; // check lifespan of this (will be copied on return?)
	//osc::OutboundPacketStream oscData;
	osc::OutboundPacketStream oscData( oscBuff, 1000 );
    oscData << osc::BeginBundle()
    		<< osc::BeginMessage( tag.c_str() )
    		<< ctrl << value
    		<< osc::EndMessage
    		<< osc::EndBundle;

    LOGD("OSC - %s",tag.c_str());

    return oscData;
}

void MultiSender::DestroyMidiConnection()
{    
    midiConnection->DetachListenerConnectionLost();
    Scdf::MidiOutConnection::Destroy(midiConnection);
    midiConnection = NULL;
}

s_bool MultiSender::SetMidiOutIndex(s_int32 index)
{
	if (midiConnection) {
        DestroyMidiConnection();
	}

	if (index == -1) {
		lastOpenedMidiOutIndex = -1;
		return true;
	}

	midiConnection = Scdf::MidiOutConnection::Create(index);
	midiConnection->AttachListenerConnectionLost(this);

	lastOpenedMidiOutIndex = midiConnection ? index : -1;

	return midiConnection != NULL;
}

void MultiSender::SetMidiMessageType(MidiMessageType type)
{
	midiMsgType = type;
}

void MultiSender::SetMidiChannel(s_int32 chan)
{
	midiChannel = chan;
}

void MultiSender::SetMidiControl(s_int32 ctrl)
{
	midiControl = ctrl;
}

s_bool MultiSender::SendValue(s_int32 number, s_int32 value)
{
	SetMidiControl(number);
	return SendValue(value);
}

s_bool MultiSender::DoSendValue(s_int32 value, s_int32 channel)
{
    switch (midiMsgType) {
        case NoteOn:		return midiConnection->SendNoteOn(GetMidiPitch(),value,channel);
        case NoteOff:		return midiConnection->SendNoteOff(GetMidiPitch(),value,channel);
        case ControlChange: return midiConnection->SendControlChange(GetMidiControl(),value,channel);
        case ProgramChange: return midiConnection->SendProgramChange(value,channel);
        case PitchBend: 	return midiConnection->SendModWheel(value,channel);
        case Aftertouch: 	return midiConnection->SendAftertouch(value,channel);
        case PolyKeyPressure: return midiConnection->SendPolyKeyPressure(GetMidiPitch(),value,channel);
        default: return false;
    }
    return false;
}

s_bool MultiSender::SendValue(s_int32 value)
{
	if (oscEnabled) {
#ifndef _DEBUG
		osc::OutboundPacketStream oscData = PackOSCValue(GetMidiControl(),value,GetOscTag());
        udpSender.SendData(oscData.Data(), oscData.Size(),0);
#else
        
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define POLY_KEY_PRESSURE 0xA0
#define CONTROL_CHANGE 0xB0
#define PROGRAM_CHANGE 0xC0
#define PITCH_BENDER 0xD0
#define CHANNEL_PRESSURE 0xE0
#define CHANNEL_MASK 0xF
        scdf::RTPMIDIPacket p;
        char channelInt8 = midiChannel&CHANNEL_MASK;
        char statusByte = NOTE_ON|channelInt8;
        
        const s_byte status = statusByte;
        const s_byte dataByte1    = GetMidiPitch();
        const s_byte dataByte2 = value;
        static int sequence=0;
        const s_byte midiMessage[]  = { status, dataByte1, dataByte2 };
        p.Create(0, sequence++, sequence, (s_byte*)&midiMessage[0], 3);
        s_byte *rtp_packet;
        int packetLength=p.GetPacket(&rtp_packet);
        udpSender.SendData((s_char*)rtp_packet, packetLength,0);
        delete [] rtp_packet;
#endif
	}

	if (!midiConnection)
		return false;

    s_bool ret=true;
    if (-1!=midiChannel)
        ret=DoSendValue(value, midiChannel);
    else
    {
        const int num_MIDI_channels=16;
        for (int i=0;i<num_MIDI_channels;++i)
            ret=DoSendValue(value, i);
    }
	return ret;

}



void showTestAlert(std::string message);

void MultiSender::OnConnectionLost(Scdf::MidiOutConnection* connection)
{
	if (connection==midiConnection) {
        //DestroyMidiConnection();
		lastOpenedMidiOutIndex = -1;
        
        showTestAlert("Multi Sender OnConnectionLost");
	}
}
