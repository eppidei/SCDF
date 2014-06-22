/* SensorAudioInputImplAndroid.cpp
 *  Created on: Jun 22, 2014
 */

#include "SensorAudioInputImplAndroid.h"

scdf::SensorAudioInputImpl::SensorAudioInputImpl()
{

}

scdf::SensorAudioInputImpl::~SensorAudioInputImpl()
{

}


s_bool scdf::SensorAudioInputImpl::Setup(SensorSettings& settings)
{
    // negotiate the settings, if any of them is not supported,
    // modify its value in settings and return false
    // try starting the sensor, return result
    return true;
}

s_bool scdf::SensorAudioInputImpl::Start()
{
    return true;
}

s_bool scdf::SensorAudioInputImpl::Stop()
{
    return true;
}

