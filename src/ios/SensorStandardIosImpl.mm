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
#include "CustomPipe.h"
#include "PipesManager.h"
#include "SensorsManager.h"

using namespace scdf;

CMMotionManager *SensorStandardImpl::motionManager=NULL;


@interface TimerIos : NSObject
{
    NSTimeInterval updateInterval;
    NSTimer *timerProx;
    SensorStandardImpl *sensorRef;
}

@end

@implementation TimerIos

- (id) init
{
    timerProx = nil;
    updateInterval = 0;
    return self;
}

- (NSTimeInterval) GetTimeInterval
{
    if(updateInterval)
        return updateInterval;
    if(timerProx)
        return [timerProx timeInterval];
    
    return -1;
}

- (void) setSensorRef: (SensorStandardImpl  *) _sensorRef
{
    sensorRef = _sensorRef;
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
    
    sensorRef->MySensorsCallback(data);
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

s_uint64 MillisecondsTo_mach_timebase(double timeToConvert_ms)
{
    const int64_t kOneMillion = 1000 * 1000;
    static mach_timebase_info_data_t s_timebase_info;
    
    if (s_timebase_info.denom == 0) {
        (void) mach_timebase_info(&s_timebase_info);
    }
    
    return (s_uint64)(timeToConvert_ms * (float)(kOneMillion * s_timebase_info.denom))/(float)(s_timebase_info.numer);
}


s_bool scdf::SensorStandardImpl::IsAvailable(SensorType type)
{
	switch (type){

		case Accelerometer:
		case Gyroscope:
		case Magnetometer:
		case Proximity:
			return true;
		default:
			return false;
	}
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
            LOGD("toggle Accelerometer Sensor ON \n");
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
            LOGD("toggle Gyroscope Sensor ON \n");
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
            LOGD("toggle Magnetometer Sensor ON \n");
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
            LOGD("toggle Proximity Sensor ON \n");
            UIDevice *device = [UIDevice currentDevice];
            device.proximityMonitoringEnabled = YES;
            BOOL proximityEnabled = device.proximityMonitoringEnabled;
            if(!proximityEnabled)
            {
                
                return false;
            }
            
            
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
            LOGD("toggle Accelerometer Sensor OFF \n");
            [motionManager stopAccelerometerUpdates];
            break;
        case scdf::Gyroscope:
            LOGD("toggle Gyroscope Sensor OFF \n");
            [motionManager stopGyroUpdates];
            break;
        case scdf::Magnetometer:
            LOGD("toggle Magnetometer Sensor OFF \n");
            [motionManager stopMagnetometerUpdates];
            break;
        case scdf::Proximity:
        {
             LOGD("toggle Proximity Sensor OFF \n");
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

s_int32 SensorStandardImpl::GetRate()
{
    NSTimeInterval updateSensorInterval;
    switch (sensorTypeRef) {
        case scdf::Accelerometer:
            updateSensorInterval=motionManager.accelerometerUpdateInterval;
            break;
        case scdf::Gyroscope:
            updateSensorInterval=motionManager.gyroUpdateInterval;
            break;
        case scdf::Magnetometer:
            updateSensorInterval=motionManager.magnetometerUpdateInterval;
            break;
        case scdf::Proximity:
            updateSensorInterval = [timerProximity GetTimeInterval];
        default:
            break;
    }

    return (s_int32) 1/updateSensorInterval;
}

s_int32 SensorStandardImpl::GetNumFramesPerCallback()
{
    return 1;
}

s_int32 SensorStandardImpl::GetNumChannels()
{
    switch (sensorTypeRef) {
        case scdf::Accelerometer:
        case scdf::Gyroscope:
        case scdf::Magnetometer:
            return 3;
            break;
        default:
            return 1;
            break;
    }
    return 1;
}

void SensorStandardImpl::MySensorsCallback(SensorsStandardIOSData &sensorIOSData)
{
    SensorData *s=thePipesManager()->ReadFromReturnPipe(sensorTypeRef);
    
#ifdef LOG_PIPES_STATUS
    LOGD("Return pipe size of %s: %d\n", SensorTypeString[sensorTypeRef].c_str(), (*(thePipesManager()->GetReturnPipes()))[sensorTypeRef]->GetSize());
#endif
    
    ((s_sample*)(s->data))[0]  = sensorIOSData.value1;
    
    if(sensorTypeRef!=Proximity)
    {
        ((s_sample*)(s->data))[1]  = sensorIOSData.value2;
        ((s_sample*)(s->data))[2]  = sensorIOSData.value3;
    }
    
    s->num_frames = GetNumFramesPerCallback();
    s->num_channels = GetNumChannels();
    s->type = sensorTypeRef;
    s->rate = GetRate();
    s->timestamp[0]=mach_absolute_time();
    s->timeid=mach_absolute_time()-MillisecondsTo_mach_timebase((1.0/GetRate())*1000);
        
    AddIncomingDataToQueue(s);
}


