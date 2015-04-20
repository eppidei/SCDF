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
    
    void MidiOutConnection::AttachListenerConnectionLost( MidiOutConnection* connection,MidiConnectionListener* _listener)
    {
        listenersMap[connection] = _listener;
    }
    
    void MidiOutConnection::DetachListenerConnectionLost(MidiOutConnection* connection)
    {
        listenersMap.erase(connection);
    }
    
    void MidiOutConnection::NotifyListenerConnectionLost(MidiOutConnection *_connection)
    {
        auto it = listenersMap.find(_connection);
        it->second->OnConnectionLost(_connection);
    
    }
    
}
