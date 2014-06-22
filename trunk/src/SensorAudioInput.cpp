
#include "SensorAudioInput.h"
#include "SensorAudioInputIosImpl.h"

using namespace scdf;

SensorAudioInput::SensorAudioInput(SensorType _type/*, ScdfPipe *destPipe*/)
{
    // if sensor is not available return null
    // otherwise, create it
    SetType(_type);
    sImpl = new SensorAudioInputImpl(_type/*,destPipe*/);
    // ios specific setup, store the pipe
    // return the just created sensor
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return  sImpl->Setup(settings);
}

s_bool scdf::SensorAudioInput::Start()
{
    return sImpl->Start();
}

s_bool scdf::SensorAudioInput::Stop()
{
    return sImpl->Stop();
}