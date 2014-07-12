
#include "SensorAudioInput.h"
#ifndef ANDROID
#include "SensorAudioInputIosImpl.h"
#else
#include "SensorAudioInputImplAndroid.h"
#endif
#include "PipesManager.h"
#include "Harvester.h"

using namespace scdf;

SensorAudioInput::SensorAudioInput(SensorType _type)
{
    SetType(_type);
    SetActive(false);
    sImpl = new SensorAudioInputImpl();
}

s_bool scdf::SensorAudioInput::Setup(SensorSettings& settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    s_bool ret = sImpl->Setup(settings);
    thePipesManager()->InitReturnPipes(GetType(), ((SensorAudioSettings&)settings).bufferSize);
    ((SensorAudioSettings&)settings).numChannels=1;
    
    return ret;
}

s_bool scdf::SensorAudioInput::Start()
{
	s_bool ret = sImpl->Start();
	if (ret)
    {
        if (Harvester::Instance()->GetType()==GetType())
            Harvester::Instance()->Start();
        SetActive(true);
    }
	return ret;
}

s_bool scdf::SensorAudioInput::Stop()
{
	s_bool ret = sImpl->Stop();
	if (ret)
    {
        if (Harvester::Instance()->GetType()==GetType())
            Harvester::Instance()->Stop();
		SetActive(false);
    }
	return ret;
}

s_int32 scdf::SensorAudioInput::GetRate()
{
    return sImpl->GetRate();
}

s_int32 scdf::SensorAudioInput::GetNumSamples()
{
    return sImpl->GetNumSamples();
}

s_int32 scdf::SensorAudioInput::GetNumChannels()
{
    return sImpl->GetNumChannels();
}

s_int32 scdf::SensorAudioInput::GetBufferSize()
{
    return sImpl->GetBufferSize();
}
