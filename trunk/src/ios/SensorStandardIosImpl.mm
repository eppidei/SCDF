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
using  scdf::SensorStandard;
using  scdf::SensorType;

SensorStandardImpl::SensorStandardImpl(SensorType type)
{
    switch (type) {
        case scdf::Accelerometer:
            sensorImpl = [[AccelerometerIos alloc] init];
            break;
        case scdf::Gyroscope:
            sensorImpl = [[GyroscopeIos alloc] init];
            break;
        case scdf::Magnetometer:
            sensorImpl = [[MagnetometerIos alloc] init];
            break;
        case scdf::Proximity:
            // TODO;
            break;
            
        default:
            sensorImpl = nil;
            break;
    }
}

SensorStandardImpl::~SensorStandardImpl()
{
    if(sensorImpl){
        [sensorImpl release];
        sensorImpl = nil;
    }
}

s_bool SensorStandardImpl::SetupImpl(scdf::SensorSettings settings)
{
    [sensorImpl Setup:settings];
    return true;
}

s_bool SensorStandardImpl::Start()
{
    return [sensorImpl Start];
}

s_bool SensorStandardImpl::Stop()
{
    return [sensorImpl Stop];
}

