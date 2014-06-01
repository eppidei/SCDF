#include "Sensor.h"

Sensor* scdf::Sensor::Create(SensorType type, Pipe destPipe)
{
    if (type == Sensor::Invalid)
        return null;
    else if (type == Sensor::AudioInput)
        return SensorAudioInput::Create(destPipe);
    else
        return SensorStandard::Create(type,destPipe);
}

SensorType scdf::Sensor::GetType(SensorData* data)
{
    return type;
}

void scdf::Sensor::AddIncomingDataToQueue(SensorData* data)
{
    // add data to the queue that has been passed at creation time...
}
