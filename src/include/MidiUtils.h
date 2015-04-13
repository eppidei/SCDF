/*
 * MidiUtils.h
 *
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#ifndef MIDIUTILS_H_
#define MIDIUTILS_H_

namespace ScdfCtrl{
enum MidiMessageType {
	Invalid = 0x0,
	NoteOn = 0x90,
	NoteOff = 0x80,
	Aftertouch = 0xA0,
	ControlChange = 0xB0,
	ProgramChange = 0xC0,
	PolyKeyPressure = 0xD0,
	PitchBend = 0xE0,
	NonMusical = 0xF0,
    MidiMessageTypeCount
};
}
#endif /* MIDIUTILS_H_ */
