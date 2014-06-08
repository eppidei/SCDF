//
//  SensorStandard.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#import "SensorStandardIosImpl.h"

using namespace scdf;

SensorStandard::SensorStandard(SensorType type)
{
    sImpl=new SensorStandardImpl(type/*,destPipe*/);
    // ios specific setup, store the pipe
    // return the just created sensor
}

s_bool SensorStandard::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return sImpl->Setup(settings);
}

s_bool SensorStandard::Start()
{
    return sImpl->Start();
}

s_bool SensorStandard::Stop()
{
    return sImpl->Stop();
}