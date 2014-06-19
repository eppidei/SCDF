//
//  AccelerometerIos.mm
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
                                            [self outputAccelertionData:accelerometerData];
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
    NSTimeInterval updateInterval = settings.rate/1000.f;
    motionManager.accelerometerUpdateInterval = updateInterval; //must be in second
    return true;
}

-(void)outputAccelertionData:(CMAccelerometerData*)accelerometerData
{
    s_double* data = new s_double[3];
    data[0]  = accelerometerData.acceleration.x;
    data[1]  = accelerometerData.acceleration.y;
    data[2]  = accelerometerData.acceleration.z;
    
    scdf::SensorData *sData = new scdf::SensorData();
    
    sData->type = scdf::Accelerometer;
    sData->data = (char*)data;
    sData->timestamp=accelerometerData.timestamp;
    
    _sensorRef->AddIncomingDataToQueue(sData);
}


@end
