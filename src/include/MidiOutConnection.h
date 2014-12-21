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

namespace Scdf  {
    
    class MidiOutConnection
    {
    public:
        class Listener
        {
        public:
            virtual void OnConnectionLost(MidiOutConnection* connection) = 0;
        };
        
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
        
        void SetListener(Listener* _listener){listener = _listener;}
        
    protected:
        
        Listener* listener;
    };

    
}



#endif
