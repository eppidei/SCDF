/*
 * MultiSender.h
 *
 *  Created on: 21 Feb 2015
 *      Author: athos
 */

#ifndef MULTISENDER_H_
#define MULTISENDER_H_


#include "TypeDefinitions.h"
#include "MidiUtils.h"
#include "MidiOutConnection.h"
#include <string>
#include "UDPSender.h"

#include "cereal/archives/xml.hpp"
#include "cereal/access.hpp"
#include "cereal/types/string.hpp"

namespace ScdfCtrl {

class MultiSender : public Scdf::MidiOutConnection::Listener {

public:

	MultiSender();

	// Get Properties:

	std::string GetOscIp();
	s_int32 GetOscPort();
	std::string GetOscTag();
	s_bool IsOscEnabled();

	s_int32 GetMidiOutIndex();
	MidiMessageType GetMidiMessageType();
	s_int32 GetMidiChannel();

	s_int32 GetMidiControl();
	s_int32 GetMidiPitch() { return GetMidiControl(); }
    s_int32 GetValue() { return val;}

	// Set Properties:

	void SetOscIp(std::string ip);
	void SetOscPort(s_int32 port);
	void SetOscEnabled(s_bool enabled);

	s_bool SetMidiOutIndex(s_int32 index);
	void SetMidiMessageType(MidiMessageType type);
	void SetMidiChannel(s_int32 chan);

	void SetMidiControl(s_int32 ctrl);
	void SetMidiPitch(s_int32 pitch) { SetMidiControl(pitch); }

	// Actions:

	s_bool SendValue(s_int32 value);
	s_bool SendValue(s_int32 number, s_int32 value);

	// Listening callbacks:

	 void OnConnectionLost(Scdf::MidiOutConnection* connection);

private:

	static osc::OutboundPacketStream PackOSCValue(s_int32 ctrl, s_int32 value, std::string tag);

	Scdf::MidiOutConnection* midiConnection;
	int lastOpenedMidiOutIndex; // TODO: ask directly to the midi connection

	// osc sender

	scdf::UDPSender udpSender;
	//std::string oscIp;
	//s_int32 oscPort;
	std::string oscTag;
	s_bool oscEnabled;

	// midi parameters:

	MidiMessageType midiMsgType;
	s_int32 midiChannel;
	s_int32 midiControl; // when midiMsgType is noteon/off, this is the pitch
    s_int32 val;

	friend class cereal::access;

	template <class Archive>
	void save( Archive & ar, std::uint32_t const version ) const
	{
		ar(CEREAL_NVP(oscTag));
		ar(CEREAL_NVP(oscEnabled));
		ar(CEREAL_NVP(midiMsgType));
		ar(CEREAL_NVP(midiChannel));
		ar(CEREAL_NVP(midiControl));
		ar(CEREAL_NVP(val));
//		ar(CEREAL_NVP(udpSender));
		ar(CEREAL_NVP(lastOpenedMidiOutIndex));
	}

	template <class Archive>
	void load( Archive & ar, std::uint32_t const version )
	{
		ar(CEREAL_NVP(oscTag));
		ar(CEREAL_NVP(oscEnabled));
		ar(CEREAL_NVP(midiMsgType));
		ar(CEREAL_NVP(midiChannel));
		ar(CEREAL_NVP(midiControl));
		ar(CEREAL_NVP(val));
//		ar(CEREAL_NVP(udpSender));

		ar(CEREAL_NVP(lastOpenedMidiOutIndex));
		SetMidiOutIndex(lastOpenedMidiOutIndex); // creates the midi connection!!!
	}

};

}


CEREAL_CLASS_VERSION(ScdfCtrl::MultiSender,0)


#endif /* MULTISENDER_H_ */
