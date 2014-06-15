//
//  SensorAudioInputIosImpl.h
//  SCDF_Test


#import "Sensor.h"
#import "SensorAudioInput.h"
#import "SensorAudioInputIos.h"

namespace scdf {
    
    class SensorAudioInputImpl : public SensorAudioInput
    {
        SensorAudioInputdIos *sensorImpl;
        
    public:
        
        SensorAudioInputImpl(SensorType type);
        ~SensorAudioInputImpl();
        
        s_bool SetupImpl(scdf::SensorSettings settings);
        s_bool Start();
        s_bool Stop();
        
    };
}

