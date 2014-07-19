//
//  SensorsManager.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//


#include "Sensor.h"
#include "SensorsManager.h"
#include "Logging.h"
#include "SensorAudioInput.h"

using namespace scdf;

/*SensorsManager *scdf::theSensorManager()
{
    static SensorsManager *instance=new SensorsManager();
    return instance;
}*/

SensorsManager *scdf::theSensorManager()
{
    static SensorsManager *instance=new SensorsManager();
    return instance;
}

void SensorsManager::SetRate(SensorType type, s_int32 rate)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor) return;
    bool wasActive=sensor->IsActive();
    if (wasActive)
        sensor->Stop();
    if(type==AudioInput)
    {
        SensorAudioInput *audioSensor=dynamic_cast<SensorAudioInput*>(sensor);
        SensorAudioSettings settings;
        settings.rate = rate;
        settings.numChannels = audioSensor->GetNumChannels();
        settings.bufferSize = audioSensor->GetBufferSize();
        sensor->Setup(settings);
    } else
    {
        SensorSettings settings;
        settings.rate = rate;
        sensor->Setup(settings);
    }
    if (wasActive)
        sensor->Start();
}

void SensorsManager::SetBufferSize(SensorType type, s_int32 size)
{
    SensorAudioInput *sensor=(SensorAudioInput *)GetSensor(type);
    if(NULL==sensor||type!=AudioInput) return;
    
    bool wasActive=sensor->IsActive();
    if (wasActive)
        sensor->Stop();
    
    SensorAudioSettings settings;
    settings.rate = sensor->GetRate();
    settings.numChannels = sensor->GetNumChannels();
    settings.bufferSize = size;

    sensor->Setup(settings);
    
    if (wasActive)
        sensor->Start();
}

s_int32 SensorsManager::GetRate(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor) return -1;
    return sensor->GetRate();
}

s_int32 SensorsManager::GetNumSamples(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor) return -1;
    return sensor->GetNumSamples();
}

s_bool SensorsManager::SensorActivated(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor) return false;
    return sensor->IsActive();
}

Sensor *SensorsManager::GetSensor(SensorType type)
{
    SensorsIterator it=sensors.find(type);
    if (it!=sensors.end())
        return it->second;
    return NULL;
}

s_bool SensorsManager::StartSensor(SensorType type)
{
    if (sensors.count(type)==0)
    	return false;
	Sensor *sensor=GetSensor(type);
    if (NULL==sensor)
        	return false;
    return sensor->Start();
}

s_bool SensorsManager::StopSensor(SensorType type)
{
	 if (sensors.count(type)==0)
	   	return false;

	Sensor *sensor=GetSensor(type);

	if (NULL==sensor)
	   	return false;

    return sensor->Stop();
}

Sensor *SensorsManager::CreateSensor(SensorType type)
{
    Sensor *s=Sensor::Create(type);
    if (NULL==s)
    {
        LOGD("Error creating sensor");
        return NULL;
    }
    sensors.insert(SensorPair(type,s));
    return s;
}


s_bool SensorsManager::DestroySensor(SensorType type)
{
	if (sensors.count(type))
		return false;

	Sensor* s = GetSensor(type);
    if (NULL==s)
    	return false;

    s->Stop();
	sensors.erase(type);
	return true;
}


s_bool SensorsManager::InitSensor(SensorType type, SensorSettings &settings)
{
	if (0==sensors.count(type)) { // sensor has not been created
		settings.broken = true;
		return false;
	}
    Sensor *s=GetSensor(type);
    if (NULL==s) return false;
	return s->Setup(settings);
}

s_bool SensorsManager::IsSensorAvailable(SensorType type)
{
	return Sensor::IsAvailable((SensorType)type);
}

void SensorsManager::CreateAllSensors() // creates all AVAILABLE sensors
{
    for (int i=Accelerometer ; i<NumTypes; i++)	{
    	if (Sensor::IsAvailable((SensorType)i))
    		CreateSensor((SensorType)i);
    }
    // don't setup sensors yet. if you try starting them before setup
    // just return false.
}

void SensorsManager::DestroyAllSensors() // destroys all CREATED sensors
{
	SensorsIterator it;

	for (it=sensors.begin(); it!=sensors.end(); it++) {
		it->second->Stop();
		delete it->second; // delete sensor, don't remove map item to not screw-up the iterator
	}
	sensors.clear(); // now we can safely clear the map
}

s_bool SensorsManager::StopAllSensors() // stops all created sensors
{
    wereActive.clear();
	SensorsIterator it;
	s_bool allok = true;
	for (it=sensors.begin(); it!=sensors.end(); it++) {
        if (it->second->IsActive())
            wereActive.push_back(it->second->GetType());
		if (!it->second->Stop())
			allok = false;
	}
	return allok; // if one of the stops failed, we report it
	// TODO: decide whether to return true or false when no sensors have been created. (now returns true)
}

s_bool SensorsManager::StartAllSensors() // starts all created sensors
{
	SensorsIterator it;
	s_bool allok = true;
	for (it=sensors.begin(); it!=sensors.end(); it++) {
		if (!it->second->Start())
			allok = false;
	}
	return allok; // if one of the starts failed, we report it
	// TODO: decide whether to return true or false when no sensors have been created. (now returns true)
}

s_bool SensorsManager::StartPrecActiveSensors() // starts all created sensors
{
	SensorsIterator it;
	s_bool allok = true;
	for (it=sensors.begin(); it!=sensors.end(); it++) {
        bool wasActive=false;
        for (int i=0;i<wereActive.size();++i){
            if (wereActive[i]==it->second->GetType())
                wasActive=true;
        }
        if (!wasActive) continue;
		if (!it->second->Start())
			allok = false;
	}
	return allok; // if one of the starts failed, we report it
	// TODO: decide whether to return true or false when no sensors have been created. (now returns true)
}



