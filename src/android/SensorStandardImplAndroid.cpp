#include "SensorStandardImplAndroid.h"

scdf::SensorStandardImpl::SensorStandardImpl(SensorType _type)
{

}

scdf::SensorStandardImpl::~SensorStandardImpl()
{

}

s_bool scdf::SensorStandardImpl::Setup(SensorSettings& settings)
{
    return true;
}

s_bool scdf::SensorStandardImpl::Start()
{
    return true;
}

s_bool scdf::SensorStandardImpl::Stop()
{
    return true;
}
