//
//  MidiOutConnection.cpp
//  ScdfController
//
//  Created by Marco Bertola on 21/12/14.
//
//

#include <stdio.h>
#include "MidiInConnection.h"
#include "MidiLearnReceiver.h"


namespace Scdf {
    
    void MidiInConnection::NotifyReceiveMidiData(MidiInData dataReceived)
    {
        MidiLearnReceiver::Instance()->OnMidiInDataReceived(&dataReceived);
    }
        
}
