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
    motionManager = [SensorStandardIos InitMotionManager];
    
    return self;
}


- (s_bool) Start
{
    [motionManager startGyroUpdatesToQueue:[NSOperationQueue currentQueue]
                                    withHandler:^(CMGyroData *gyroData, NSError *error) {
                                        [self outputRotationData:gyroData];
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
    _sensorRef = settings.sensorRef;
    
    
    return true;
}

-(void)outputRotationData:(CMGyroData *) gyroData
{
    
    s_double* data = new s_double[3];
    data[0]  = gyroData.rotationRate.x;
    data[1]  = gyroData.rotationRate.y;
    data[2]  = gyroData.rotationRate.z;
    
    scdf::SensorData *sData = new scdf::SensorData();
    
    sData->type = scdf::Gyroscope;
    sData->data = (char*)data;
    sData->timestamp=gyroData.timestamp;
    
    _sensorRef->AddIncomingDataToQueue(sData);
}


@end
