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
            
}

s_bool SensorAudioInputImpl::SetupImpl(scdf::SensorSettings settings)
{
    return true;
}

s_bool SensorAudioInputImpl::Start()
{
    return false;
}

s_bool SensorAudioInputImpl::Stop()
{
    return false;
}
