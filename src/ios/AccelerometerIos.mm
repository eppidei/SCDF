//
//  AccelerometerIos.m
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "AccelerometerIos.h"
#import "Sensor.h"


@implementation AccelerometerIos


- (id) init
{
    motionManager = [SensorStandardIos InitMotionManager];
    
    return self;
}

- (s_bool) Start
{
    [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                        withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error) {
                                            [self outputAccelertionData:accelerometerData.acceleration];
                                            if(error){
                                                
                                                NSLog(@"%@", error);
                                            }
                                        }];
    return true;

}

- (s_bool) Stop
{
    [motionManager stopAccelerometerUpdates];
    return true;
    
}
- (s_bool) Setup:(scdf::SensorSettings) settings
{
    NSTimeInterval updateInterval = settings.rate;
    motionManager.accelerometerUpdateInterval = updateInterval;
    
    
    
    return false;
}

-(void)outputAccelertionData:(CMAcceleration)acceleration
{
    s_double xValue=  acceleration.x;
    s_double yValue=  acceleration.y;
    s_double zValue=  acceleration.z;
    
    s_double* accelerometerData = new s_double[3];
    accelerometerData[0]  = xValue;
    accelerometerData[1]  = yValue;
    accelerometerData[2]  = zValue;
    
    scdf::SensorData data;
    
    data.type = scdf::Accelerometer;
    data.data = accelerometerData;
    
    sensorRef->AddIncomingDataToQueue(&data);
    
    delete accelerometerData;
}


@end
