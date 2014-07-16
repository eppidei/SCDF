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
    s_bool wasActive;
}

- (void)Attach:(scdf::SensorAudioInputImpl *)_audioSensor;
@end

namespace scdf {
    
    class SensorAudioInputImpl : public SensorAudioInput
    {
        void SetupIOUnit(scdf::SensorAudioSettings &settings);
        void InitAudioSession();
        void InitIOUnit();
        
        AudioEventsListener *listener;
        AudioUnit rioUnit;
        
        //s_int32 currentSampleRate;
        
    public:
        static OSStatus PerformRender (void                         *inRefCon,
                                       AudioUnitRenderActionFlags 	*ioActionFlags,
                                       const AudioTimeStamp 		*inTimeStamp,
                                       UInt32 						inBusNumber,
                                       UInt32 						inNumberFrames,
                                       AudioBufferList              *ioData);
        SensorAudioInputImpl();
        ~SensorAudioInputImpl();
        
        s_bool Setup(scdf::SensorSettings &settings);
        s_bool Start();
        s_bool Stop();
        s_int32 GetRate();
        s_int32 GetNumSamples();
        s_int32 GetNumChannels();
        s_int32 GetBufferSize() ;
        
    };
}