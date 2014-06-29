//
//  Harvester.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "CustomPipe.h"
#include "ThreadUtils.h"
#include "UDPSender.h"
#include "UDPSendersManager.h"

using namespace scdf;
extern std::vector<scdf::CustomPipe*> pipes;
extern std::vector<scdf::CustomPipe*> returnPipes;

Harvester *Harvester::_instance=NULL;

s_uint64 getUptimeInMilliseconds(s_uint64 timeToConvert);

s_bool Harvester::Compare::operator()(const SensorData* s1, const SensorData* s2) const
{
    return s1->timestamp < s2->timestamp;
}

static void StartHarvestingProcedure(void *param)
{
    Harvester *harvester=((Harvester*)param);
    while(harvester->activated)
    {
        SensorData *data=pipes[harvester->GetType()]->ReadMessage<SensorData*>();
        if (NULL!=data)
            harvester->HarvestingProcedure(data);
    }
}

void Harvester::SetupPipes()
{
    for (int i=0;i<pipes.size();++i)
        pipes[i]->SetNonBlockingReads();
}

Harvester::Harvester() : activated(true)
{
    SetType(AudioInput);
    SetupPipes();
    ThreadUtils::CreateThread((start_routine)StartHarvestingProcedure, this);
}

void Harvester::SetType(SensorType type)
{
    requesterType=type;
}

void Harvester::Sort()
{
    if (myHarvest.size()==0) return;
    std::sort(myHarvest.begin(), myHarvest.end(), Compare());
}

void Harvester::PipesHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd, SensorType sType)
{
    for (s_int32 i=0;i<pipes.size();++i)
    {
        if (i==sType) continue;
        SensorData *sd=NULL;
        while (1)
        {
            sd=pipes[i]->ReadMessage<SensorData*>();
            if (NULL==sd) break;
            if (sd->timestamp<timestampStart)
            {
#ifdef LOG_HARVEST_STATUS
                LOGD("DELETING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
                if (0==returnPipes[sd->type]->WriteMessage<SensorData*>(sd))
                    delete sd;
#ifdef LOG_PIPES_STATUS
                LOGD("Return pipe size of %s: %d\n", SensorTypeString[sd->type].c_str(), returnPipes[sd->type]->GetSize());
#endif
            }
            
            else if (sd->timestamp<=timestampEnd){
#ifdef LOG_HARVEST_STATUS
                printf("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
                sd->timeid=timestampStart;
                myHarvest.push_back(sd);
            }
            else{
#ifdef LOG_HARVEST_STATUS
                printf("ADDING TO NEXT: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
                nextHarvestData.push_back(sd);
                break;
            }
        }
    }
}

void Harvester::InternalBufferHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd)
{
    SensorData *sd=NULL;
    for (s_int32 i=0;i<nextHarvestData.size();++i)
    {
        sd=nextHarvestData[i];
        if (sd->timestamp<timestampStart)
        {
#ifdef LOG_HARVEST_STATUS
            printf("DELETING: Timestamp from %s: %lu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
            if (0==returnPipes[sd->type]->WriteMessage<SensorData*>(sd))
                delete sd;
#ifdef LOG_PIPES_STATUS
            LOGD("Return pipe size of %s: %d\n", SensorTypeString[sd->type].c_str(), returnPipes[sd->type]->GetSize());
#endif
            nextHarvestData.erase(nextHarvestData.begin()+i);
        }
        else if (sd->timestamp<=timestampEnd)
        {
#ifdef LOG_HARVEST_STATUS
            printf("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
            myHarvest.push_back(nextHarvestData[i]);
            nextHarvestData.erase((nextHarvestData.begin()+i));
        }
    }
}

void Harvester::NotifyHarvestCompletition()
{
    UDPSendersManager::Instance()->GetActiveSender()->EventFreeSlot()->Wait();
    //InterlockedExchange
    myHarvest.swap(UDPSendersManager::Instance()->GetActiveSender()->senderData);
    UDPSendersManager::Instance()->GetActiveSender()->EventCanSend()->Set();
}

void Harvester::HarvestingProcedure(SensorData *masterData)
{
    Harvest(masterData);
#ifdef LOG_DATA
    for (int i = 0; i< masterData->num_samples; i ++) {
        printf("Harvested data %d from %s: %.4f\n",i,SensorTypeString[masterData->type].c_str(), ((s_sample*)masterData->data)[i]);
    }
#endif
    NotifyHarvestCompletition();
}

void Harvester::Harvest(SensorData *masterData)
{
#ifdef LOG_TIMESTAMP
    printf("Master sensor: %s; Harvesting starts: %llu; Harvesting ends: %llu; Harvesting ms interval: %llu;\n",SensorTypeString[masterData->type].c_str(),masterData->timeid,masterData->timestamp,getUptimeInMilliseconds(masterData->timestamp-masterData->timeid));
#endif
    InternalBufferHarvesting(masterData->timeid, masterData->timestamp);
    PipesHarvesting(masterData->timeid, masterData->timestamp, masterData->type);
    Sort();
    myHarvest.insert(myHarvest.begin(), masterData);
}
