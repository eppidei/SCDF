
#include "CustomPipe.h"
#include "Harvester.h"
#include "Sensor.h"
#include "SensorAudioInput.h"
#include "SensorStandard.h"
#include "PipesManager.h"


using namespace scdf;



s_bool Sensor::IsAvailable(SensorType type)
{
	if (type==AudioInput)
		return true; // always present by design for both android and ios
	return SensorStandard::IsAvailable(type);
}


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
    if (data->type>=thePipesManager()->NumPipes())
        delete data;
    else
        thePipesManager()->WriteOnPipe(data->type,data);
    
    if(0!=thePipesManager()->GetPipes()->size()&&(*(thePipesManager()->GetPipes()))[data->type]->GetSize()>1000)
        StopRestartMachine kk;
#ifdef LOG_DATA
    for (int i = 0; i< data->num_samples; i ++) {
        printf("Collected data %d from %s: %.4f\n",i,SensorTypeString[data->type].c_str(), ((s_float*)data->data)[i]);
    }
#endif
    
#ifdef LOG_PIPES_STATUS
    if(0!=thePipesManager()->GetPipes()->size())
        LOGD("Sensor %s pipe size: %d\n", SensorTypeString[data->type].c_str(), (*(thePipesManager()->GetPipes()))[data->type]->GetSize());
#endif

}

s_bool Sensor::IsActive()
{
    return active;
}
void Sensor::SetActive(s_bool _active)
{
    active=_active;
}
