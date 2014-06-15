
#include <vector>
#include "CustomPipe.h"
#include "Harvester.h"
#include "Sensor.h"
#include "SensorAudioInput.h"
#include "SensorStandard.h"


using namespace scdf;
extern std::vector<scdf::CustomPipe*> pipes;

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

#define TEST

void Sensor::AddIncomingDataToQueue(SensorData* data)
{
    // add data to the queue that has been passed at creation time...
#ifndef TEST
    pipes[GetType()]->WriteMessage<SensorData*>(data);
#else
    s_float *mydata = (s_float *)data->data;
    
    for (int i = 0; i< 512; i ++) {
        printf("Value Data 1: %.4f\n",  mydata[i]);
    }
    //printf("Value Data 1: %.4f\n, Value Data 2: %.4f\n,Value Data 3 %.2f \n\n", mydata[0],mydata[1], mydata[2]);
    delete data;
#endif

}

void Sensor::Harvest(SensorData* masterData)
{
    Harvester::Instance()->Harvest(masterData);
}
