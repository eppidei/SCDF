#include "../SensorStandard.h

SensorStandard* scdf::SensorStandard::Create(SensorType type, Pipe destPipe)
{
    // if sensor is not available return null
    // otherwise, create it 
    SensorStandard* s = new SensorStandard();    
    // android specific setup, store the pipe
    // return the just created sensor
    return s;
}

s_bool scdf::SensorStandard::Setup(SensorSettings* settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return 
}

s_bool scdf::SensorStandard::Start()
{
    return true;
}

s_bool scdf::SensorStandard::Stop()
{
    return true;
}
