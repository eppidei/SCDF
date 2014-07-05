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
#include "PipesManager.h"

using namespace scdf;

s_bool SensorStandard::IsAvailable(SensorType type)
{
	return SensorStandardImpl::IsAvailable(type);
}

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
    thePipesManager()->InitReturnPipes(GetType(), num_samples);
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

s_int32 SensorStandard::GetRate()
{
    return sImpl->GetRate();
}

s_int32 SensorStandard::GetNumSamples()
{
    return sImpl->GetNumSamples();
}
