#include "MidiInConnection.h"

// midi in:

s_int32 Scdf::MidiInConnection::GetNumAvailableInputs()
{
	return 0;
}

std::string Scdf::MidiInConnection::GetInputName(s_int32 index)
{
	return "...";
}
        
Scdf::MidiInConnection* Scdf::MidiInConnection::Create(s_int32 index)
{
	return NULL;
}

void Scdf::MidiInConnection::Destroy(MidiInConnection* connection)
{

}
        
void Scdf::MidiInConnection::NotifyReceiveMidiData(MidiInData dataReceived)
{

}
