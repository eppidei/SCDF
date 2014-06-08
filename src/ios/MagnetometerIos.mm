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
                                            [self outputMagnetometedData:magneticFieldData.magneticField];
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
    
    
    
    return false;
}

-(void)outputMagnetometedData:(CMMagneticField)magneticField
{
    s_double xValue=  magneticField.x;
    s_double yValue=  magneticField.y;
    s_double zValue=  magneticField.z;
    
    scdf::SensorData data;
    sensorRef->AddIncomingDataToQueue(&data);
    

}


@end
