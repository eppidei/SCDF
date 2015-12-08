//
//  MidiLearnReceiver.cpp
//  ScdfController
//
//  Created by marcobertola on 08/12/15.
//
//

#include "MidiLearnReceiver.h"
#include "MidiInConnection.h"
#include "Logging.h"



namespace Scdf {
    
    
    MidiLearnReceiver *MidiLearnReceiver::_instance=NULL;
    
    MidiLearnReceiver::MidiLearnReceiver()
    {
        isEnable = false;
    }
    
    void MidiLearnReceiver::ToogleMidiLearn(bool on)
    {
        LOGD("MidiLearnReceiver::ToogleMidiLearn \n");
        
        isEnable = on;
        
        if(on)
        {
            for (int i=0;i<Scdf::MidiInConnection::GetNumAvailableInputs();++i)
            midiInputconnections.push_back(MidiInConnection::Create(i));
        } else
        {
            for (int i=0;i<midiInputconnections.size();++i)
                (MidiInConnection::Destroy(midiInputconnections[i]));
            midiInputconnections.clear();
        }
        
    }
    
    void MidiLearnReceiver::OnMidiInDataReceived(Scdf::MidiInData *data)
    {
         LOGD("OnMidiInDataReceived Status: %d, Data: %d, Data, %d \n", data->statusByte, data->dataByte1, data->dataByte2);
    }
    
}