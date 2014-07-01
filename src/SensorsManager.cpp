//
//  SensorsManager.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//



#include "Sensor.h"
#include "SensorsManager.h"
#include "Logging.h"

using namespace scdf;

SensorsManager *scdf::theSensorManager()
{
    static SensorsManager *instance=new SensorsManager();
    return instance;
}

s_int32 SensorsManager::GetRate(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    return sensor->GetRate();
}

s_int32 SensorsManager::GetNumSamples(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    return sensor->GetNumSamples();
}

Sensor *SensorsManager::GetSensor(SensorType type)
{
    SensorsIterator it=sensors.find(type);
    if (it!=sensors.end())
        return it->second;
    return NULL;
}

void SensorsManager::ActivateSensor(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor){
        sensor=Sensor::Create(type);
        if (NULL==sensor) return;
    }
    sensor->Start();
}

void SensorsManager::DeActivateSensor(SensorType type)
{
    Sensor *sensor=GetSensor(type);
    if (NULL!=sensor)
        sensor->Stop();
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

void SensorsManager::InitSensor(SensorType type, SensorSettings &settings)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor){
        sensor=CreateSensor(type);
        if (NULL==sensor) return;
    }
    sensor->Setup(settings);
}

void SensorsManager::CreateAllSensor()
{
    
    // NB: Remember to add sensors in sensorList vector in StopAllSensors();
    
    CreateSensor(scdf::AudioInput);
    CreateSensor(scdf::Accelerometer);
    CreateSensor(scdf::Gyroscope);
    CreateSensor(scdf::Magnetometer);
    CreateSensor(scdf::Proximity);
    
    
    scdf::SensorSettings s_settings;
    s_settings.rate= (s_int32) DEFAULT_SENORS_STANDARD_RATE;
    InitSensor(scdf::Accelerometer,s_settings);
    InitSensor(scdf::Magnetometer,s_settings);
    InitSensor(scdf::Gyroscope,s_settings);
    InitSensor(scdf::Proximity,s_settings);
    
    
    scdf::SensorAudioSettings audioSettings;
    audioSettings.bufferSize= (s_int32) DEFAULT_AUDIO_BUFFER_SIZE;
    InitSensor(scdf::AudioInput,audioSettings);
    
#ifdef ANDROID
    
    // create other android sensors
    
#endif
}

void SensorsManager::StopAllSensors()
{
    std::vector<Sensor *> sensorsList;
    
    sensorsList.push_back(GetSensor(AudioInput));
    sensorsList.push_back(GetSensor(Accelerometer));
    sensorsList.push_back(GetSensor(Magnetometer));
    sensorsList.push_back(GetSensor(Gyroscope));
    sensorsList.push_back(GetSensor(Proximity));
   
    
    for(int i = 0;i<sensorsList.size();i++)
        sensorsList[i]->Stop();
    
}

