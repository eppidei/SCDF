#include "ScdfPipe.h"
#include "Sensor.h"
#include "SensorAudioInput.h"
#include "SensorStandard.h"

using namespace scdf;

Sensor* Sensor::Create(SensorType type, ScdfPipe *destPipe)
{
    if (type == SensorType::Invalid)
        return NULL;
    else if (type == SensorType::AudioInput)
        return SensorAudioInput::Create(type,destPipe);
    else
        return SensorStandard::Create(type,destPipe);
}

SensorType Sensor::GetType()
{
    return type;
}

void Sensor::AddIncomingDataToQueue(SensorData* data)
{
    // add data to the queue that has been passed at creation time...
}
