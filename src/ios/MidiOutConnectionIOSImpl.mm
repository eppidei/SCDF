
#import "MidiOutConnectionIOSImpl.h"
#import <mach/mach_time.h>
#import "Label.h"
#import "MidiClientIOS.h"





#pragma mark MidiOutConnectionIOS impl

#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define POLY_KEY_PRESSURE 0xA0
#define CONTROL_CHANGE 0xB0
#define PROGRAM_CHANGE 0xC0
#define PITCH_BENDER 0xD0
#define CHANNEL_PRESSURE 0xE0

#define CHANNEL_MASK 0xF
#define DATA_BYTE_MASK 0x7F

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
        char statusByte = NOTE_ON|channelInt8;
        
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
        char statusByte = NOTE_OFF|channelInt8;

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
        char statusByte = CONTROL_CHANGE|channelInt8;
        
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
        char statusByte = PROGRAM_CHANGE|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = program;
        
        const UInt8 midiMessage[]  = { status, dataByte1 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;
    }
    s_bool MidiOutConnectionIOSImpl::SendAftertouch(s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = CHANNEL_PRESSURE|channelInt8;
        
        const UInt8 status = statusByte;
        UInt8 dataByte1    = value;
        
        const UInt8 midiMessage[]  = { status, dataByte1 };
        
        [getMidiManager() sendBytes:midiMessage size:sizeof(midiMessage) atPort:indexMidiOutPort];
        
        return true;

    }
    s_bool MidiOutConnectionIOSImpl::SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel)
    {
        char channelInt8 = channel&CHANNEL_MASK;
        char statusByte = POLY_KEY_PRESSURE|channelInt8;
        
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
        char statusByte = PITCH_BENDER|channelInt8;
        
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

