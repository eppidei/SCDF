#include "../SensorStandard.h"

SensorAudioInput* scdf::SensorAudioInput::Create(SensorSettings* settings, Pipe destPipe)
{
    // if sensor is not available return null
    // otherwise, create it 
    SensorAudioInput* s = new SensorAudioInput();    
    // android specific setup, store the pipe
    // return the just created sensor
    return s;
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings* settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return 
}

s_bool scdf::SensorAudioInput::Start()
{
    return true;
}

s_bool scdf::SensorAudioInput::Stop()
{
    return true;
}
