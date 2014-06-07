//
//  SensorStandardIos.cpp
//
//  Created by Marco Bertola on 07/06/14.
//
//

#include "TypeDefinitions.h"
#include "../SensorStandard.h"
#include "../ScdfPipe.h"

using namespace scdf;

SensorStandard* scdf::SensorStandard::Create(SensorType type, ScdfPipe *destPipe)
{
    // if sensor is not available return null
    // otherwise, create it 
    SensorStandard* s = new SensorStandard();    
    // ios specific setup, store the pipe
    // return the just created sensor
    return s;
}

s_bool scdf::SensorStandard::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return true;
}

s_bool scdf::SensorStandard::Start()
{
    return true;
}

s_bool scdf::SensorStandard::Stop()
{
    return true;
}
