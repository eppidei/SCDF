/*
 * ControlUnit.cpp
 *
 *  Created on: 21 Dec 2014
 *      Author: athos
 */

using namespace ScdfCtrl;

std::string ControlUnit::GetOscIp()
{
	return "sto.caz.zo!";
}

s_int32 ControlUnit::GetOscPort()
{
	return 5555;
}

std::string ControlUnit::GetOscTag()
{
	return "/sto/caz/zo";
}

s_bool ControlUnit::IsOscEnabled()
{
	return true;
}

s_int32 ControlUnit::GetMidiOutIndex()
{
	return 0;
}

s_int32 ControlUnit::GetMidiMessageType()
{
	return 0x80;
}

s_int32 ControlUnit::GetMidiChannel()
{
	return 15;
}

s_int32 ControlUnit::GetMidiControl()
{
	return 62;
}

s_int32 ControlUnit::GetMidiPitch()
{
	return GetMidiControl();
}
