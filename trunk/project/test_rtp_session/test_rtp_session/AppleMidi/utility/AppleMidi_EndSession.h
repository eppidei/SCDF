/*!
 *  @file		AppleMIDI_EndSession.h
 *  Project		Arduino AppleMIDI Library
 *	@brief		AppleMIDI Library for the Arduino 
 *	Version		0.3
 *  @author		lathoub 
 *	@date		04/04/14
 *  License		GPL
 */

#pragma once

#include "AppleMidi_Settings.h"
#include "AppleMidi_Defs.h"
#include "AppleMidi_Util.h"

BEGIN_APPLEMIDI_NAMESPACE
	
class AppleMIDI_EndSession {
public:
	uint8_t		signature[2];
	uint8_t		command[2];
	uint32_t	version;
	uint32_t	initiatorToken;
	uint32_t	ssrc;


	inline AppleMIDI_EndSession()
	{
		init();
	}

private:
	void init()
	{
		memcpy(signature, amSignature, sizeof(amSignature));
		memcpy(command, amEndSession, sizeof(amEndSession));
	}
};

END_APPLEMIDI_NAMESPACE
