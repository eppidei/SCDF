//
//  PipesManager.cpp
//
//  Created by Andrea Scuderi on 01/06/14.
//
//

#include "CustomPipe.h"
#include "PipesManager.h"
#include "Harvester.h"
#include "SensorsManager.h"

using namespace scdf;

void PipesManager::CreatePipes()
{
    for (int i=0;i<NumTypes;++i)
    {
        CustomPipe *p=new CustomPipe();
        if (i!=Harvester::Instance()->GetType()/* || !Harvester::Instance()->IsAudioSyncActive()*/)
            p->SetNonBlockingReads();
        pipes.push_back(p);
    }
}

void PipesManager::CreateReturnPipes()
{
    for (int i=0;i<NumTypes;++i)
    {
        CustomPipe *p=new CustomPipe();
        p->SetNonBlockingReads();
        p->SetNonBlockingWrites();
        returnPipes.push_back(p);
    }
}

std::vector<CustomPipe*>* PipesManager::GetReturnPipes()
{
    return &returnPipes;
}

std::vector<CustomPipe*>* PipesManager::GetPipes()
{
    return &pipes;
}

void PipesManager::InitReturnPipe(SensorType type/*, s_int32 numSamples*/)
{
    while(true)
    {
        SensorData *s=returnPipes[type]->ReadMessage<SensorData*>();
        if (NULL==s) break;
        delete s;
    }
    for (int j=0;j<RETURN_PIPES_STATIC_INIT;++j)
    {
        SensorData *s=new SensorData(type);
        s_int32 size=theSensorManager()->GetNumChannels(type)*theSensorManager()->GetNumFramesPerCallback(type);
        s->data=new s_sample[size];
        s->timestamp = new s_uint64[1];
        if (0==returnPipes[type]->WriteMessage<SensorData*>(s))
            delete s;
    }
}

void PipesManager::InitReturnPipes()
{
    for (int i=0;i<NumTypes;++i)
        InitReturnPipe((SensorType)i);
}

void PipesManager::InitPipes()
{
    for (int i=0;i<pipes.size();++i)
    {
        for (int j=0;j<pipes[i]->GetSize();++j)
        {
            SensorData *s=pipes[i]->ReadMessage<SensorData*>();
            if (s!=NULL)
                delete s;
        }
        pipes[i]->Close();
        delete pipes[i];
    }
    pipes.clear();
    CreatePipes();
    
}

SensorData *PipesManager::ReadFromPipe(SensorType type)
{
	return pipes[type]->ReadMessage<SensorData*>();
}

s_int32 PipesManager::WriteOnReturnPipe(SensorType type, SensorData *data)
{
    return returnPipes[type]->WriteMessage<SensorData*>(data);
}

SensorData *PipesManager::ReadFromReturnPipe(SensorType type)
{
    SensorData *data=returnPipes[type]->ReadMessage<SensorData*>();
    if (NULL==data)
    {
        data = new scdf::SensorData(type);
        data->timestamp = new s_uint64[1];
        s_int32 size=theSensorManager()->GetNumFramesPerCallback(type)*theSensorManager()->GetNumChannels(type);
        data->data=(s_sample*) new s_sample[size];
    }
    return data;
}

s_int32 PipesManager::NumPipes()
{
    return (s_int32)pipes.size();
}

s_int32 PipesManager::WriteOnPipe(SensorType type, SensorData *data)
{
	assert(data!=NULL);
	return pipes[type]->WriteMessage<SensorData*>(data);
}

PipesManager *scdf::thePipesManager()
{
    static PipesManager *instance=new PipesManager();
    return instance;
}

