#include "Sensor.h"

class Pipe;

namespace scdf {

    class AudioSettings : public SensorSettings {
    
    public:    
        s_int32 framesPerBuffer;
        s_int16 channels;
    };

    class AudioInputData : public SensorData {
    public: 
        s_sample* buffer;
        s_int32 framesPerBuffer;
    };


    class SensorAudioInput : public Sensor {
    public:
        static SensorAudioInput* Create(SensorType type, ScdfPipe *destPipe);
        s_bool Setup(SensorSettings settings);        
        s_bool Start();
        s_bool Stop();

    };

}
