//
//  SensorAudioInputIosImpl.h
//  SCDF_Test


#import "Sensor.h"
#import "SensorAudioInput.h"
#import "SensorStandardIos.h"

namespace scdf {
    
    class SensorAudioInputImpl : public SensorAudioInput
    {
        SensorStandardIos *sensorImpl;
        
    public:
        
        SensorAudioInputImpl(SensorType type);
//        SensorAudioInputImpl();
        s_bool SetupImpl(scdf::SensorSettings settings);
        s_bool Start();
        s_bool Stop();
        
    };
}

