//
//  GyroscopeIos.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "GyroscopeIos.h"

@implementation GyroscopeIos

- (id) init
{
    motionManager = [SensorStandardIosImpl InitMotionManager];
    
    return self;
}

- (void)   AssingPipe: (scdf::ScdfPipe *) destPipe
{
    pipe = *destPipe;
}

- (s_bool) Start
{
    [motionManager startGyroUpdatesToQueue:[NSOperationQueue currentQueue]
                                    withHandler:^(CMGyroData *gyroData, NSError *error) {
                                        [self outputRotationData:gyroData.rotationRate];
                                    }];
    return true;
    
}

- (s_bool) Stop
{
    [motionManager stopGyroUpdates];
    return true;
    
}
- (s_bool) Setup:(scdf::SensorSettings) settings
{
    NSTimeInterval updateInterval = settings.rate;
    motionManager.gyroUpdateInterval = updateInterval;
    
    
    
    return false;
}

-(void)outputRotationData:(CMRotationRate)rotation
{
    s_double xValue=  rotation.x;
    s_double yValue=  rotation.y;
    s_double zValue=  rotation.z;
}


@end
