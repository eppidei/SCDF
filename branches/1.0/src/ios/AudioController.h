//
//  AudioController.h
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>
#import "TypeDefinitions.h"
#import "Sensor.h"


@interface AudioController : NSObject {
    
    AudioUnit               _rioUnit;
    scdf::Sensor            *_sensorRef;
}


- (s_bool) Start;
- (s_bool) Stop ;
- (s_bool) Setup:(scdf::SensorSettings) setting;

@end
