//
//  MidiLearnReceiver.hpp
//  ScdfController
//
//  Created by marcobertola on 08/12/15.
//
//

#ifndef MidiLearnReceiver_hpp
#define MidiLearnReceiver_hpp

#include <stdio.h>
#include <vector>



namespace Scdf {
    
    struct MidiInData;
    class MidiInConnection;
    
    class MidiLearnReceiver
    {
        bool isEnable;
        static MidiLearnReceiver *_instance;
        
        std::vector<MidiInConnection*> midiInputconnections;
        
    public:
        MidiLearnReceiver();
        bool IsEnable(){return isEnable;}
        static MidiLearnReceiver *Instance()
        {
            if (NULL==_instance) _instance=new MidiLearnReceiver();
            return _instance;
        }
        
        void ToogleMidiLearn(bool on);
        
        void OnMidiInDataReceived(Scdf::MidiInData *data);
        
    };
}


#endif /* MidiLearnReceiver_hpp */
