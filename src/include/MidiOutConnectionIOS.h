//
//  MIDIOutConnection.h
//  ScdfController
//
//  Created by Marco Bertola on 21/12/14.
//
//

#ifndef ScdfController_MIDIOutConnection_h
#define ScdfController_MIDIOutConnection_h

#include "MidiOutConnection.h"



namespace Scdf {
    
    class MidiOutConnectionIOSImpl;
    
    
    class MidiOutConnectionIOS : public MidiOutConnection
    {
        
    private:
        MidiOutConnectionIOSImpl* midiImpl;
        
    public:
        
        MidiOutConnectionIOS(int _currentMIDIOutPortIndex);
        ~MidiOutConnectionIOS();
        
        
        s_bool SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        
        s_bool SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel) {return false;}
        s_bool SendProgramChange(s_uint16 program, s_uint16 channel) {return false;}
        s_bool SendAftertouch(s_uint16 value, s_uint16 channel){return false;}
        s_bool SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel) {return false;}
        s_bool SendModWheel(s_uint16 value, s_uint16 channel) {return false;}
        
    };
    
}


#endif
