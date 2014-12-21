/*
 * ControlUnit.h
 *
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

#ifndef CONTROLUNIT_H_
#define CONTROLUNIT_H_

#include "TypeDefinitions.h"

namespace ScdfCtrl {

class ControlUnit {

public:

	std::string GetOscIp();
	s_int32 GetOscPort();
	std::string GetOscTag();
	s_bool IsOscEnabled();

	s_int32 GetMidiOutIndex();
	s_int32 GetMidiMessageType();
	s_int32 GetMidiChannel();

	s_int32 GetMidiControl();
	s_int32 GetMidiPitch();

};


}


#endif /* CONTROLUNIT_H_ */
