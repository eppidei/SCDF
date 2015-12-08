//
//  MidiOutConnection.h
//  SCDF_Test
//
//  Created by Marco Bertola on 14/12/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//


#include "TypeDefinitions.h"
#include <vector>

namespace Scdf  {
    
    class MidiInConnection;
    
    
    struct MidiInData
    {
        s_uint16 statusByte;
        s_uint16 dataByte1;
        s_uint16 dataByte2;
    };
    
    class MidiInConnection
    {
    protected:
        
        
    public:
        
        static s_int32 GetNumAvailableInputs();
        static std::string GetInputName(s_int32 index);
        
        static MidiInConnection* Create(s_int32 index);
        static void Destroy(MidiInConnection* connection);
        
        static void NotifyReceiveMidiData(MidiInData dataReceived);
        
        
    };


    
}

