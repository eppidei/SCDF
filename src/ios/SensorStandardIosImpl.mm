//
//  SensorStandardIosImpl.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIosImpl.h"
#import "AccelerometerIos.h"
#import "MagnetometerIos.h"
#import "GyroscopeIos.h"

using namespace scdf;

CMMotionManager *SensorStandardImpl::motionManager=NULL;

SensorStandardImpl::SensorStandardImpl()
{
    if (NULL==motionManager)
        motionManager = [[CMMotionManager alloc] init];
}

SensorStandardImpl::~SensorStandardImpl()
{
/*    assert(false);
    if (NULL!=motionManager)
        [motionManager release];
    motionManager=NULL;*/
}

s_bool SensorStandardImpl::Setup(scdf::SensorSettings settings)
{
    NSTimeInterval updateInterval = 1.0/(s_float)settings.rate;
    switch (type) {
        case scdf::Accelerometer:
            motionManager.accelerometerUpdateInterval = updateInterval;
            break;
        case scdf::Gyroscope:
            motionManager.gyroUpdateInterval = updateInterval;
            break;
        case scdf::Magnetometer:
            motionManager.magnetometerUpdateInterval = updateInterval;
            break;
        default:
            break;
    }
    return true;
}

s_bool SensorStandardImpl::Start()
{
    switch (type) {
        case scdf::Accelerometer:
            [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                        withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error) {
                                            SensorsStandardIOSData data;
                                            data.x=accelerometerData.acceleration.x;
                                            data.y=accelerometerData.acceleration.y;
                                            data.z=accelerometerData.acceleration.z;
                                            data.timestamp=accelerometerData.timestamp;
                                            MySensorsCallback(data);
                                            if(error){
                                                NSLog(@"%@", error);
                                            }
                                        }];
            break;
        case scdf::Gyroscope:
            [motionManager startGyroUpdatesToQueue:[NSOperationQueue currentQueue]
                                       withHandler:^(CMGyroData *gyroData, NSError *error) {
                                           SensorsStandardIOSData data;
                                           data.x=gyroData.rotationRate.x;
                                           data.y=gyroData.rotationRate.y;
                                           data.z=gyroData.rotationRate.z;
                                           data.timestamp=gyroData.timestamp;
                                           MySensorsCallback(data);
                                           if(error){
                                               NSLog(@"%@", error);
                                           }
                                       }];
            break;
        case scdf::Magnetometer:
            [motionManager startMagnetometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                        withHandler:^(CMMagnetometerData  *magneticFieldData, NSError *error) {
                                            SensorsStandardIOSData data;
                                            data.x=magneticFieldData.magneticField.x;
                                            data.y=magneticFieldData.magneticField.y;
                                            data.z=magneticFieldData.magneticField.z;
                                            data.timestamp=magneticFieldData.timestamp;
                                            MySensorsCallback(data);
                                            if(error){
                                                NSLog(@"%@", error);
                                            }
                                        }];
            break;
        default:
            break;
    }
    return true;
}

s_bool SensorStandardImpl::Stop()
{
    switch (type) {
        case scdf::Accelerometer:
            [motionManager stopAccelerometerUpdates];
            break;
        case scdf::Gyroscope:
            [motionManager stopGyroUpdates];
            break;
        case scdf::Magnetometer:
            [motionManager stopMagnetometerUpdates];
            break;
        default:
            break;
    }
    return true;
}

void SensorStandardImpl::MySensorsCallback(SensorsStandardIOSData &sensorIOSData)
{
    s_double* data = new s_double[3];
    data[0]  = sensorIOSData.x;
    data[1]  = sensorIOSData.y;
    data[2]  = sensorIOSData.z;
        
    scdf::SensorData *sData = new scdf::SensorData();
        
    sData->type = scdf::Magnetometer;
    sData->data = (char*)data;
    sData->timestamp=sensorIOSData.timestamp;
        
    AddIncomingDataToQueue(sData);
}

