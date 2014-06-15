//
//  MagnetometerIos.m
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "MagnetometerIos.h"

@implementation MagnetometerIos

- (id) init
{
    motionManager = [SensorStandardIos InitMotionManager];
    
    return self;
}

- (s_bool) Start
{
    [motionManager startMagnetometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                        withHandler:^(CMMagnetometerData  *magneticFieldData, NSError *error) {
                                            [self outputMagnetometedData:magneticFieldData];
                                            if(error){
                                                
                                                NSLog(@"%@", error);
                                            }
                                        }];
    return true;
    
}

- (s_bool) Stop
{
    [motionManager stopMagnetometerUpdates];
    return true;
    
}
- (s_bool) Setup:(scdf::SensorSettings) settings
{
    NSTimeInterval updateInterval = settings.rate;
    motionManager.accelerometerUpdateInterval = updateInterval;
    _sensorRef = settings.sensorRef;
    
    return true;
}

-(void)outputMagnetometedData:(CMMagnetometerData *)magneticFieldData
{
    s_double* data = new s_double[3];
    data[0]  = magneticFieldData.magneticField.x;
    data[1]  = magneticFieldData.magneticField.y;
    data[2]  = magneticFieldData.magneticField.z;
    
    scdf::SensorData *sData = new scdf::SensorData();
    
    sData->type = scdf::Magnetometer;
    sData->data = (char*)data;
    sData->timestamp=magneticFieldData.timestamp;
    
    _sensorRef->AddIncomingDataToQueue(sData);
    
}


@end
