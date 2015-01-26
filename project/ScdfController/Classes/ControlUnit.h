/*
 * ControlUnit.h
 *
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#ifndef CONTROLUNIT_H_
#define CONTROLUNIT_H_

#include "TypeDefinitions.h"
#include "MidiUtils.h"
#include "MidiOutConnection.h"
#include <string>

namespace ScdfCtrl {

class ControlUnit : public Scdf::MidiOutConnection::Listener {

public:

	ControlUnit();

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
    s_int32 GetValue() { return value;}

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

	Scdf::MidiOutConnection* midiConnection;
	int lastOpenedMidiOutIndex; // TODO: ask directly to the midi connection

	// osc sender

	std::string oscIp;
	s_int32 oscPort;
	std::string oscTag;
	s_bool oscEnabled;

	MidiMessageType midiMsgType;
	s_int32 midiChannel;
	s_int32 midiControl; // when midiMsgType is noteon/off, this is the pitch
    s_int32 value;


};


}


#endif /* CONTROLUNIT_H_ */
