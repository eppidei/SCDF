
#include <vector>
#include "ScdfPipe.h"
#include "Sensor.h"
#include "SensorAudioInput.h"
#include "SensorStandard.h"


using namespace scdf;
extern std::vector<scdf::ScdfPipe> pipes;

Sensor* Sensor::Create(SensorType type)
{
    if (type == SensorType::Invalid)
        return NULL;
    else if (type == SensorType::AudioInput)
        return new SensorAudioInput(type);
    else
        return new SensorStandard(type);
}

SensorType Sensor::GetType()
{
    return type;
}

void Sensor::AddIncomingDataToQueue(SensorData* data)
{
    // add data to the queue that has been passed at creation time...
    pipes[GetType()].WriteMessage<SensorData*>(data);
}
