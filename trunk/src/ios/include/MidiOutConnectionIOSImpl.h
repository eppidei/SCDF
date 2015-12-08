

#import <UIKit/UIKit.h>
#import <CoreMIDI/CoreMIDI.h>
#include "MidiOutConnection.h"
#include "MidiInConnection.h"
#include "MidiOutConnectionIOS.h"



namespace Scdf {
    
        
  
    
    class MidiOutConnectionIOSImpl : public MidiOutConnection
    {
        
    private:
        
        int indexMidiOutPort;
        
    public:
        
        MidiOutConnectionIOSImpl(int _currentMIDIOutPortIndex);
        ~MidiOutConnectionIOSImpl();
        
        
        static s_int32 GetNumAvailableOutputs();
        static std::string GetOutputName(s_int32 index);
        
        s_bool SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel);
        s_bool SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel);
        s_bool SendProgramChange(s_uint16 program, s_uint16 channel);
        s_bool SendAftertouch(s_uint16 value, s_uint16 channel);
        s_bool SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel);
        s_bool SendModWheel(s_uint16 value, s_uint16 channel);
        
        void AttachListenerConnectionLost(MidiConnectionListener* _listener);
        void DetachListenerConnectionLost();

    };
    
    class MidiInConnectionIOSImpl : public MidiInConnection
    {
        
    private:
        
        int indexMidiInPort;
        
    public:
        
        MidiInConnectionIOSImpl(int _currentMIDIInPortIndex);
        ~MidiInConnectionIOSImpl();
        
        
        static s_int32 GetNumAvailableInputs();
        static std::string GetInputName(s_int32 index);
        
    };
    
    
}
