
#include "CustomPipe.h"
#include "Harvester.h"
#include "Sensor.h"
#include "SensorAudioInput.h"
#include "SensorStandard.h"


using namespace scdf;
extern vector<scdf::CustomPipe*> pipes;

Sensor* Sensor::Create(SensorType type)
{
    if (type == Invalid)
        return NULL;
    else if (type == AudioInput)
        return new SensorAudioInput(type);
    else
        return new SensorStandard(type);
}


void Sensor::AddIncomingDataToQueue(SensorData* data)
{
    // add data to the queue that has been passed at creation time...
    pipes[data->type]->WriteMessage<SensorData*>(data);
    
#ifdef TEST_PRINT_DATA
    for (int i = 0; i< data->num_samples; i ++) {
        printf("Collected data %d from %s: %.4f\n",i,SensorTypeString[data->type].c_str(), ((s_sample*)data->data)[i]);
    }
#endif

}
