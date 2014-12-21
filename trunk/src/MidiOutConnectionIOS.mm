//
//  MidiOutConnectionIOS.cpp
//  ScdfController
//
//  Created by Marco Bertola on 21/12/14.
//
//

#include <stdio.h>
#include "MidiOutConnectionIOSImpl.h"
#include "MidiOutConnectionIOS.h"


namespace Scdf {
    
    MidiOutConnectionIOS::MidiOutConnectionIOS(int _currentMIDIOutPortIndex)
    {
        midiImpl = new MidiOutConnectionIOSImpl(_currentMIDIOutPortIndex);
    }
    
    MidiOutConnectionIOS::~MidiOutConnectionIOS()
    {
        delete midiImpl;
    }
    
    s_int32 MidiOutConnection::GetNumAvailableOutputs()
    {
        return MidiOutConnectionIOSImpl::GetNumAvailableOutputs();
    }
    std::string MidiOutConnection::GetOutputName(s_int32 index)
    {
        return MidiOutConnectionIOSImpl::GetOutputName(index);
    }
    
    MidiOutConnection* MidiOutConnection::Create(s_int32 index)
    {
        return new MidiOutConnectionIOS(index);
    }
    void MidiOutConnection::Destroy(MidiOutConnection* connection)
    {
        MidiOutConnectionIOS *myConnection = dynamic_cast<MidiOutConnectionIOS*>(connection);
        if(myConnection)
            delete myConnection;
    }
    
    s_bool MidiOutConnectionIOS::SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel)
    {
        return midiImpl->SendNoteOn(note, velocity, channel);
    }
    s_bool MidiOutConnectionIOS::SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel)
    {
        return midiImpl->SendNoteOff(note, velocity, channel);
    }
    s_bool MidiOutConnectionIOS::SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel)
    {
        return midiImpl->SendControlChange(control, value, channel);
    }
    s_bool MidiOutConnectionIOS::SendProgramChange(s_uint16 program, s_uint16 channel)
    {
        return midiImpl->SendProgramChange(program, channel);
    }
    s_bool MidiOutConnectionIOS::SendAftertouch(s_uint16 value, s_uint16 channel)
    {
        return midiImpl->SendAftertouch(value,channel);
    }
    s_bool MidiOutConnectionIOS::SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel)
    {
        return midiImpl->SendPolyKeyPressure(note,value,channel);
    }
    s_bool MidiOutConnectionIOS::SendModWheel(s_uint16 value, s_uint16 channel)
    {
        return midiImpl->SendModWheel(value,channel);
    }
    
    
}
