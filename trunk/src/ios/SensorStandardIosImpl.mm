//
//  SensorStandardIosImpl.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIosImpl.h"
#import "AccelerometerIos.h"

using namespace scdf;
using  scdf::SensorStandard;
using  scdf::SensorType;

SensorStandardImpl::SensorStandardImpl(SensorType type)
{
    switch (type) {
        case scdf::Accelerometer:
            sensorImpl = [[AccelerometerIos alloc] init];
            //[sensorImpl AssingPipe:destPipe];
            break;
        case scdf::Gyroscope:
            //
            break;
        case scdf::Magnetometer:
            //
            break;
        case scdf::Proximity:
            //
            break;
            
        default:
            sensorImpl = nil;
            break;
    }
}

SensorStandardImpl::SensorStandardImpl()
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

