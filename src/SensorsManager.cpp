//
//  SensorsManager.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "Sensor.h"
#include "SensorsManager.h"
#include "ScdfPipe.h"
#include <vector>

using namespace scdf;

namespace scdf
{
    extern std::vector<ScdfPipe> pipes;
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
        sensor=Sensor::Create(type, &pipes[type]);
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
    Sensor *s=Sensor::Create(type, &pipes[type]);
    if (NULL==s)
    {
        //Logging::Instance("Error creating sensor");
        return NULL;
    }
    sensors.insert(SensorPair(type,s));
    return s;
}

void SensorsManager::InitSensor(SensorType type, SensorSettings settings)
{
    Sensor *sensor=GetSensor(type);
    if (NULL==sensor){
        sensor=CreateSensor(type);
        if (NULL==sensor) return;
    }
    sensor->Setup(settings);
}

