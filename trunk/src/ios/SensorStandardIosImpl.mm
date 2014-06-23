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


@interface TimerIos : NSObject
{
    s_float updateInterval;
    NSTimer *timerProx;
    SensorStandardImpl *senorRef;
}

@end

@implementation TimerIos

- (id) init
{
    timerProx = nil;
    return self;
}

- (void) setSensorRef: (SensorStandardImpl  *) _sensorRef
{
    senorRef = _sensorRef;
}


- (void) Setup: (scdf::SensorSettings) settings
{
    updateInterval = 1.0/ (s_float) settings.rate;
    
}

- (void) Start
{
    if (!timerProx) {
        timerProx = [NSTimer scheduledTimerWithTimeInterval:updateInterval
                                         target:self
                                       selector:@selector(timerFired:)
                                       userInfo:nil
                                        repeats:YES];
    }
}

- (void) Stop
{
    if(timerProx)
    {
        [timerProx invalidate];
        timerProx = nil;
    }
}

- (void) timerFired: (NSTimer *) timer
{
    UIDevice *device = [UIDevice currentDevice];
    SensorsStandardIOSData data;
    data.value1 = device.proximityState;
    data.timestamp = [timer timeInterval];
    
    senorRef->MySensorsCallback(data);
}

@end

#include <mach/mach_time.h>
s_uint64 getUptimeInMilliseconds(s_uint64 timeToConvert)
{
    const int64_t kOneMillion = 1000 * 1000;
    static mach_timebase_info_data_t s_timebase_info;
    
    if (s_timebase_info.denom == 0) {
        (void) mach_timebase_info(&s_timebase_info);
    }
    
    // mach_absolute_time() returns billionth of seconds,
    // so divide by one million to get milliseconds
    return (s_uint64)((timeToConvert * s_timebase_info.numer) / (kOneMillion * s_timebase_info.denom));
}

SensorStandardImpl::SensorStandardImpl(SensorType _sensorType)
{
    sensorTypeRef = _sensorType;
    if (NULL==motionManager)
        motionManager = [[CMMotionManager alloc] init];
    
    if (_sensorType==Proximity) {
        timerProximity = [[TimerIos alloc] init];
        [timerProximity setSensorRef:this];
    }
}

SensorStandardImpl::~SensorStandardImpl()
{
/*    assert(false);
    if (NULL!=motionManager)
        [motionManager release];
    motionManager=NULL;*/
    
    if(timerProximity)
    {
        [timerProximity release];
        timerProximity = nil;
    }
}

s_bool SensorStandardImpl::Setup(scdf::SensorSettings settings)
{
    NSTimeInterval updateInterval = 1.0/(s_float)settings.rate;
    switch (sensorTypeRef) {
        case scdf::Accelerometer:
            motionManager.accelerometerUpdateInterval = updateInterval;
            break;
        case scdf::Gyroscope:
            motionManager.gyroUpdateInterval = updateInterval;
            break;
        case scdf::Magnetometer:
            motionManager.magnetometerUpdateInterval = updateInterval;
            break;
        case scdf::Proximity:
            [timerProximity Setup:settings];
        default:
            break;
    }
    return true;
}

s_bool SensorStandardImpl::Start()
{
    switch (sensorTypeRef) {
        case scdf::Accelerometer:
            [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                        withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error) {
                                            SensorsStandardIOSData data;
                                            data.value1=accelerometerData.acceleration.x;
                                            data.value2=accelerometerData.acceleration.y;
                                            data.value3=accelerometerData.acceleration.z;
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
                                           data.value1=gyroData.rotationRate.x;
                                           data.value2=gyroData.rotationRate.y;
                                           data.value3=gyroData.rotationRate.z;
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
                                            data.value1=magneticFieldData.magneticField.x;
                                            data.value2=magneticFieldData.magneticField.y;
                                            data.value3=magneticFieldData.magneticField.z;
                                            data.timestamp=magneticFieldData.timestamp;
                                            MySensorsCallback(data);
                                            if(error){
                                                NSLog(@"%@", error);
                                            }
                                        }];
            break;
        case scdf::Proximity:
        {
            UIDevice *device = [UIDevice currentDevice];
            device.proximityMonitoringEnabled = YES;
            [timerProximity Start];
            break;
        }
        default:
            break;
    }
    return true;
}

s_bool SensorStandardImpl::Stop()
{
    switch (sensorTypeRef) {
        case scdf::Accelerometer:
            [motionManager stopAccelerometerUpdates];
            break;
        case scdf::Gyroscope:
            [motionManager stopGyroUpdates];
            break;
        case scdf::Magnetometer:
            [motionManager stopMagnetometerUpdates];
            break;
        case scdf::Proximity:
        {
            UIDevice *device = [UIDevice currentDevice];
            device.proximityMonitoringEnabled = NO;
            [timerProximity Stop];
             break;
        }
        default:
            break;
    }
    return true;
}

void SensorStandardImpl::MySensorsCallback(SensorsStandardIOSData &sensorIOSData)
{
    s_int32 numSamples = 0;
    if (sensorTypeRef==Proximity) {
        numSamples = 1;
    } else
        numSamples = 3;
    
    
    s_sample* data = new s_sample[numSamples];
    data[0]  = sensorIOSData.value1;
    
    if(sensorTypeRef!=Proximity)
    {
        data[1]  = sensorIOSData.value2;
        data[2]  = sensorIOSData.value3;
    }
    
    scdf::SensorData *sData = new scdf::SensorData();
    
    sData->num_samples = numSamples;
    sData->type = sensorTypeRef;
    sData->data = (char*)data;
//    sData->timestamp=sensorIOSData.timestamp;
    sData->timestamp=sData->timeid=mach_absolute_time();
        
    AddIncomingDataToQueue(sData);
}


