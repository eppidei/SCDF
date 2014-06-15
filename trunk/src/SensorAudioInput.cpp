
#include "SensorAudioInput.h"
#include "SensorAudioInputIosImpl.h"

using namespace scdf;

SensorAudioInput::SensorAudioInput(SensorType type/*, ScdfPipe *destPipe*/)
{
    // if sensor is not available return null
    // otherwise, create it
    sImpl = new SensorAudioInputImpl(type/*,destPipe*/);
    // ios specific setup, store the pipe
    // return the just created sensor
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return  sImpl->SetupImpl(settings);
}

s_bool scdf::SensorAudioInput::Start()
{
    return sImpl->Start();
}

s_bool scdf::SensorAudioInput::Stop()
{
    return sImpl->Stop();
}