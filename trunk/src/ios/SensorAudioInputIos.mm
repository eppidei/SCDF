//
//  SensorsAudioInputIos.cpp
//
//  Created by Marco Bertola on 07/06/14.
//
//

#include "SensorAudioInputIos.h"
#import "AVFoundation/AVAudioSession.h"

#import "AudioController.h"


@implementation SensorAudioInputdIos


- (id)init
{
    if (self = [super init]) {
        audioController = [[AudioController alloc] init];
    }
    return self;
}


- (s_bool) Start
{
    
    return [audioController Start];
}

- (s_bool) Stop
{
    return [audioController Stop];
}

- (s_bool) Setup:(scdf::SensorSettings) setting
{    
    
    return [audioController Setup:setting];
}


@end