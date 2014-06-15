//
//  SensorAudioInputImpl.mm
//  SCDF_Test
//
//  Created by Marco Bertola on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "Sensor.h"
#import "SensorAudioInputIosImpl.h"


using namespace scdf;

SensorAudioInputImpl::SensorAudioInputImpl(SensorType type/*, ScdfPipe *destPipe*/)
{
    sensorImpl = [[SensorAudioInputdIos alloc] init];
            
}

SensorAudioInputImpl::~SensorAudioInputImpl()
{
    [sensorImpl release];
}

s_bool SensorAudioInputImpl::SetupImpl(scdf::SensorSettings settings)
{
    return [sensorImpl Setup:settings];
}

s_bool SensorAudioInputImpl::Start()
{
    return [sensorImpl Start];
}

s_bool SensorAudioInputImpl::Stop()
{
    return [sensorImpl Stop];
}




