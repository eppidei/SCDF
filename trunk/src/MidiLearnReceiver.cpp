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
#include "MidiUtils.h"

#ifndef ANDROID
void OnMidiLearnReceived(ScdfCtrl::MidiMessageType message, int channel, int dataValue_01,int dataValue_02);
#else
void OnMidiLearnReceived(ScdfCtrl::MidiMessageType message, int channel, int dataValue_01,int dataValue_02)
{

}
#endif

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
            midiLearnData.reset(nullptr);
        }
        
    }
    
    void MidiLearnReceiver::OnMidiInDataReceived(Scdf::MidiInData *data)
    {
         LOGD("OnMidiInDataReceived Status: %d, Data: %d, Data, %d \n", data->statusByte, data->dataByte1, data->dataByte2);
        
        if(!isEnable)
            return;
        
        
        s_int16 channel = data->statusByte&CHANNEL_MASK;
        s_int16 message = data->statusByte&STATUS_MASK;
        ScdfCtrl::MidiMessageType msg = ScdfCtrl::NonMusical;

        
        int indexLabel = -1;
        
        switch (message) {
            case ScdfCtrl::NoteOn:
                msg = ScdfCtrl::NoteOn;
                LOGD("Received: NoteOn\n");
        
                break;
            case ScdfCtrl::NoteOff:
                 msg = ScdfCtrl::NoteOff;
                LOGD("Received: NoteOff\n");
                
                break;
            case ScdfCtrl::Aftertouch:
                 msg = ScdfCtrl::Aftertouch;
                LOGD("Received: Aftertouch\n");
                
                break;
            case ScdfCtrl::ControlChange:
                 msg = ScdfCtrl::ControlChange;
                LOGD("Received: ControlChange\n");
                
                break;
            case ScdfCtrl::ProgramChange:
                 msg = ScdfCtrl::ProgramChange;
                LOGD("Received: ProgramChange\n");
                
                break;
            case ScdfCtrl::PolyKeyPressure:
                 msg = ScdfCtrl::PolyKeyPressure;
                LOGD("Received: PolyKeyPressure\n");
                
                break;

            case ScdfCtrl::PitchBend:
                msg = ScdfCtrl::PitchBend;
                LOGD("Received: PitchBend\n");
                
                break;


                
            default:
                break;
        }
        if(msg!=ScdfCtrl::NonMusical)
            OnMidiLearnReceived(msg, channel, data->dataByte1,data->dataByte2);
        
    }
    
}
