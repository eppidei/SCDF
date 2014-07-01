
#include "SensorAudioInput.h"
#ifndef ANDROID
#include "SensorAudioInputIosImpl.h"
#else
#include "SensorAudioInputImplAndroid.h"
#endif

using namespace scdf;

void InitReturnPipes(SensorType type, s_int32 numSamples);

SensorAudioInput::SensorAudioInput(SensorType _type)
{
    SetType(_type);
    sImpl = new SensorAudioInputImpl();
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings& settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    
    InitReturnPipes(GetType(), ((SensorAudioSettings&)settings).bufferSize);
    ((SensorAudioSettings&)settings).numChannels=1;
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

s_int32 scdf::SensorAudioInput::GetRate()
{
    return sImpl->GetRate();
}

s_int32 scdf::SensorAudioInput::GetNumSamples()
{
    return sImpl->GetNumSamples();
}
