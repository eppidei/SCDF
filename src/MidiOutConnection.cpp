//
//  MidiOutConnection.cpp
//  ScdfController
//
//  Created by Marco Bertola on 21/12/14.
//
//

#include <stdio.h>
#include "MidiOutConnection.h"


namespace Scdf {
    

    std::map<MidiOutConnection*, MidiConnectionListener*> MidiOutConnection::listenersMap = {};
    MidiDeviceMenuListener* MidiOutConnection::midiDeviceMenulistener = NULL;
    
    
    void MidiOutConnection::AttachMidiDeviceMenuListener(MidiDeviceMenuListener* _midiDeviceMenulistener )
    {
        midiDeviceMenulistener = _midiDeviceMenulistener;
    }
    
    void MidiOutConnection::NotifyMidiDeviceMenuListener()
    {
        if(midiDeviceMenulistener)
            midiDeviceMenulistener->UpdateMIDIDevicesMenu();
    }
        
}
