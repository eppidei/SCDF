
#import "MidiOutConnectionIOSImpl.h"
#import <mach/mach_time.h>
#import "Label.h"
#import "MidiClientIOS.h"
#import "MidiUtils.h"





#pragma mark MidiOutConnectionIOS impl



static
NSString *NameOfEndpoint(MIDIEndpointRef ref)
{
    CFStringRef string = nil;
    OSStatus s = MIDIObjectGetStringProperty(ref, kMIDIPropertyDisplayName, ( CFStringRef*)&string);
    if ( s != noErr )
    {
        return @"Unknown name";
    }
    return (NSString*)CFBridgingRelease(string);
}

namespace Scdf
{
    
    /// ******* MIDI IN ***********/////////
    s_int32 MidiInConnection::GetNumAvailableInputs()
    {
        return MidiInConnectionIOSImpl::GetNumAvailableInputs();
    }
    std::string MidiInConnection::GetInputName(s_int32 index)    {
        return MidiInConnectionIOSImpl::GetInputName(index);
    }
    
    MidiInConnection* MidiInConnection::Create(s_int32 index)
    {
        return new MidiInConnectionIOSImpl(index);
    }
    void MidiInConnection::Destroy(MidiInConnection* connection)
    {
        MidiInConnectionIOSImpl *myConnection = static_cast<MidiInConnectionIOSImpl*>(connection);
        if(myConnection){
            delete myConnection;
            myConnection = NULL;
        }
    }



    
    
    /// ******* MIDI OUT ***********/////////
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
        return new MidiOutConnectionIOSImpl(index);
    }
    void MidiOutConnection::Destroy(MidiOutConnection* connection)
    {
        MidiOutConnectionIOSImpl *myConnection = dynamic_cast<MidiOutConnectionIOSImpl*>(connection);
        if(myConnection)
           delete myConnection;
    }
    
    
    SCDF_Midi* midiNetworkManager = NULL;
    
    void createMidiMananager()
    {
       if(NULL==midiNetworkManager)
            midiNetworkManager = [[SCDF_Midi alloc] init];
        midiNetworkManager.networkEnabled = YES;
        
    }
    
    SCDF_Midi* getMidiManager() {return midiNetworkManager;}
    
    MidiInConnectionIOSImpl::MidiInConnectionIOSImpl(int _currentMIDIInPortIndex)
    {
        
        [getMidiManager() connectSourceEndopointAtIndex:_currentMIDIInPortIndex];

        indexMidiInPort = _currentMIDIInPortIndex;
    }
    
    MidiInConnectionIOSImpl::~MidiInConnectionIOSImpl()
    {
        [getMidiManager() disConnectSourceEndopointAtIndex:indexMidiInPort];
    }
    
    s_int32 MidiInConnectionIOSImpl::GetNumAvailableInputs()
    {
        return (s_int32)MIDIGetNumberOfSources();
    };
    
    std::string MidiInConnectionIOSImpl::GetInputName(s_int32 index)
    {
        NSString *nameOfOutput = NameOfEndpoint(MIDIGetSource(index));
        return [nameOfOutput UTF8String];
    }

    
    
    MidiOutConnectionIOSImpl::MidiOutConnectionIOSImpl(int _currentMIDIOutPortIndex)
    {
        indexMidiOutPort = _currentMIDIOutPortIndex;
    }
    
    MidiOutConnectionIOSImpl::~MidiOutConnectionIOSImpl()
    {
        
    }
    
    s_int32 MidiOutConnectionIOSImpl::GetNumAvailableOutputs()
    {
        return (s_int32)MIDIGetNumberOfDestinations();
    };
    
    std::string MidiOutConnectionIOSImpl::GetOutputName(s_int32 index)
    {
        NSString *nameOfOutput = NameOfEndpoint(MIDIGetDestination(index));
        return [nameOfOutput UTF8String];
    }
    
    s_bool MidiOutConnectionIOSImpl::SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel)
    {
        
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::NoteOn|channelInt8;
        
        const UInt8 status = statusByte;
        const UInt8 dataByte1    = note;
        const UInt8 dataByte2 = velocity;
        
        const UInt8 midiMessage[]  = { status, dataByte1, dataByte2 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        return true;
    }
    
    s_bool MidiOutConnectionIOSImpl::SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::NoteOff|channelInt8;

        const UInt8 status = statusByte;
        UInt8 dataByte1    = note;
        UInt8 dataByte2 = velocity;
        
        const UInt8 midiMessage[]  = { status, dataByte1, dataByte2 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    
    s_bool MidiOutConnectionIOSImpl::SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::ControlChange|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = control;
        UInt8 dataByte2 = value;
        
        const UInt8 midiMessage[]  = { status, dataByte1, dataByte2 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    s_bool MidiOutConnectionIOSImpl::SendProgramChange(s_uint16 program, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::ProgramChange|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = program;
        
        const UInt8 midiMessage[]  = { status, dataByte1 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    s_bool MidiOutConnectionIOSImpl::SendAftertouch(s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::Aftertouch|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = value;
        
        const UInt8 midiMessage[]  = { status, dataByte1 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;

    }
    s_bool MidiOutConnectionIOSImpl::SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::PolyKeyPressure|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = note;
        UInt8 dataByte2 = value;
        
        const UInt8 midiMessage[]  = { status, dataByte1, dataByte2 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    s_bool MidiOutConnectionIOSImpl::SendModWheel(s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = ScdfCtrl::PitchBend|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = value&DATA_BYTE_MASK;
        UInt8 dataByte2 = (value>>7)&DATA_BYTE_MASK;
        
        const UInt8 midiMessage[]  = { status, dataByte1, dataByte2 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    
    void MidiOutConnectionIOSImpl::AttachListenerConnectionLost(MidiConnectionListener* _listener)
    {
        listenerConnectionLost = _listener;
        
        [getMidiManager() attachListenerAtIndex:indexMidiOutPort forConnection:this];
    }
    
    void MidiOutConnectionIOSImpl::DetachListenerConnectionLost()
    {
        [getMidiManager() detachListenerAtIndex:indexMidiOutPort];

        listenerConnectionLost = NULL;
        
    }
    
    
}

