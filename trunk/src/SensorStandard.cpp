//
//  SensorStandard.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifdef ANDROID
#include "SensorStandardImplAndroid.h"
#else
#import "SensorStandardIosImpl.h"
#endif
#include "SensorStandard.h"

using namespace scdf;

void InitReturnPipes(SensorType type, s_int32 numSamples);

SensorStandard::SensorStandard(SensorType _type)
{
    SetType(_type);
    sImpl=new SensorStandardImpl(_type);
    // platform specific setup, store the pipe
    // return the just created sensor
}

s_bool SensorStandard::Setup(SensorSettings& settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    int num_samples=3;
    if (GetType()==Proximity)
        num_samples=1;
    InitReturnPipes(GetType(), num_samples);
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
