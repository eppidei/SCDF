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
Harvester *Harvester::_instance=NULL;

s_bool Harvester::Compare::operator()(const SensorData* s1, const SensorData* s2) const
{
    return s1->timestamp < s2->timestamp;
}

static void StartHarvestingProcedure(void *param)
{
    Harvester *harvester=((Harvester*)param);
    while(harvester->activated)
    {
        harvester->HarvestingProcedure(pipes[harvester->GetType()]->ReadMessage<SensorData*>());
    }
    delete harvester;
}

void Harvester::SetupPipes()
{
    for (int i=0;i<pipes.size();++i)
    {
        if(i==requesterType)
            pipes[i]->SetBlockingReads();
        else
            pipes[i]->SetNonBlockingReads();
    }
}

Harvester::Harvester() : activated(true),requesterType(SensorType::AudioInput)
{
    SetupPipes();
    ThreadUtils::CreateThread((start_routine)StartHarvestingProcedure, this);
}

void Harvester::Sort()
{
    if (myHarvest.size()==0) return;
    std::sort(myHarvest.begin(), myHarvest.end(), Compare());
}

void Harvester::PipesHarvesting(s_double timestampStart, s_double timestampEnd, SensorType sType)
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
                delete sd;
            else if (sd->timestamp<=timestampEnd){
                sd->timeid=timestampStart;
                myHarvest.push_back(sd);
            }
            else{
                nextHarvestData.push_back(sd);
                break;
            }
        }
    }
}

void Harvester::InternalBufferHarvesting(s_double timestampStart, s_double timestampEnd)
{
    SensorData *sd=NULL;
    for (s_int32 i=0;i<nextHarvestData.size();++i)
    {
        sd=nextHarvestData[i];
        if (sd->timestamp<timestampStart)
        {
            delete sd;
            nextHarvestData.erase(nextHarvestData.begin()+i);
        }
        else if (sd->timestamp<=timestampEnd)
        {
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
#ifdef TEST_PRINT_DATA
    for (int i = 0; i< masterData->num_samples; i ++) {
        printf("Harvested data %d from %s: %.4f\n",i,SensorTypeString[masterData->type].c_str(), ((s_sample*)masterData->data)[i]);
    }
#endif
    NotifyHarvestCompletition();
}

void Harvester::Harvest(SensorData *masterData)
{
    s_double timestampStart=masterData->timestamp;
    s_double harvestEpoc_seconds=masterData->rate;
    if (masterData->type!=AudioInput)
        timestampStart-=harvestEpoc_seconds;
    
    InternalBufferHarvesting(timestampStart, timestampStart+harvestEpoc_seconds);
    PipesHarvesting(timestampStart, timestampStart+harvestEpoc_seconds, masterData->type);
    Sort();
    myHarvest.insert(myHarvest.begin(), masterData);
}