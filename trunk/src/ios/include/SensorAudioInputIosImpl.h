//
//  SensorAudioInputIosImpl.h
//  SCDF_Test


#import "Sensor.h"
#import "SensorAudioInput.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

namespace scdf {
    class SensorAudioInputImpl;
}

@interface AudioEventsListener :  NSObject{
    
    scdf::SensorAudioInputImpl *audioSensor;
}

- (void)Attach:(scdf::SensorAudioInputImpl *)_audioSensor;
@end

namespace scdf {
    
    class SensorAudioInputImpl : public SensorAudioInput
    {
        void SetupAudioSession(scdf::SensorSettings &settings);
        void SetupIOUnit(scdf::SensorSettings &settings);
        void SetupAudioChain(scdf::SensorSettings &settings);
        
        AudioEventsListener *listener;
        AudioUnit _rioUnit;
        
    public:
        
        SensorAudioInputImpl();
        ~SensorAudioInputImpl();
        
        s_bool Setup(scdf::SensorSettings &settings);
        s_bool Start();
        s_bool Stop();
        
    };
}