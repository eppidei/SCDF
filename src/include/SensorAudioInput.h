#include "Sensor.h"

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

    class SensorAudioInputImpl;
    class SensorAudioInput : public Sensor
    {
        SensorAudioInputImpl *sImpl;
    public:
        SensorAudioInput(){}
        SensorAudioInput(SensorType type);
        s_bool Setup(SensorSettings& settings);
        s_bool Start();
        s_bool Stop();

    };

}
