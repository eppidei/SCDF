
#include "SensorAudioInput.h"
#ifndef ANDROID
#include "SensorAudioInputIosImpl.h"
#else
#include "SensorAudioInputImplAndroid.h"
#endif

using namespace scdf;

SensorAudioInput::SensorAudioInput(SensorType _type/*, ScdfPipe *destPipe*/)
{
    SetType(_type);
    sImpl = new SensorAudioInputImpl(/*,destPipe*/);
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings& settings)
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
