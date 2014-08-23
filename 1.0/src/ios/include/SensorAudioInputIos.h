//
//  SensorAudioInputIos.h
//  SCDF_Test

#import <UIKit/UIKit.h>
#import "TypeDefinitions.h"
#import "Sensor.h"

@class AudioController;


@interface SensorAudioInputdIos : NSObject

{
     scdf::Sensor  *sensorRef;
    AudioController *audioController;
    
}



- (s_bool) Start;
- (s_bool) Stop ;
- (s_bool) Setup:(scdf::SensorSettings) setting;

@end
