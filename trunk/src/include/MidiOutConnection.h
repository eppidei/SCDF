//
//  MidiOutConnection.h
//  SCDF_Test
//
//  Created by Marco Bertola on 14/12/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef SCDF_Test_MidiOutConnection_h
#define SCDF_Test_MidiOutConnection_h

#include "TypeDefinitions.h"
#include <vector>

namespace Scdf  {
    
    class MidiOutConnection;
    
    
    class MidiConnectionListener
    {
    public:
        virtual void OnConnectionLost(MidiOutConnection* connection) = 0;
    };
    
    
    class MidiDeviceMenuListener {
        
    public:
        virtual void UpdateMIDIDevicesMenu() = 0;
        
    };
    
    
    class MidiOutConnection
    {
    protected:
        
        MidiConnectionListener *listenerConnectionLost;
        
    public:
        
        MidiConnectionListener* GetListenerConnectionLost(){return listenerConnectionLost;}

        static std::map<MidiOutConnection*, MidiConnectionListener*> listenersMap;
        static MidiDeviceMenuListener *midiDeviceMenulistener;
        
        static s_int32 GetNumAvailableOutputs();
        static std::string GetOutputName(s_int32 index);
        
        static MidiOutConnection* Create(s_int32 index);
        static void Destroy(MidiOutConnection* connection);
        
        virtual s_bool SendNoteOn(s_uint16 note, s_uint16 velocity, s_uint16 channel) = 0;
        virtual s_bool SendNoteOff(s_uint16 note, s_uint16 velocity, s_uint16 channel) = 0;
        virtual s_bool SendControlChange(s_uint16 control, s_uint16 value, s_uint16 channel) = 0;
        virtual s_bool SendProgramChange(s_uint16 program, s_uint16 channel) = 0;
        virtual s_bool SendAftertouch(s_uint16 value, s_uint16 channel) = 0;
        virtual s_bool SendPolyKeyPressure(s_uint16 note, s_uint16 value, s_uint16 channel) = 0;
        virtual s_bool SendModWheel(s_uint16 value, s_uint16 channel) = 0;
        
        virtual void AttachListenerConnectionLost(MidiConnectionListener* _listener ) = 0;
        virtual void DetachListenerConnectionLost() = 0;
        
        static void AttachMidiDeviceMenuListener(MidiDeviceMenuListener* _midiDeviceMenulistener );
        static void NotifyMidiDeviceMenuListener();

    protected:
        
        
    };

    
}



#endif
