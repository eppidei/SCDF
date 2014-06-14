//
//  Harvester.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "CustomPipe.h"
#include "Utils.h"

using namespace scdf;
extern std::vector<scdf::CustomPipe> pipes;
Harvester *Harvester::_instance=NULL;

/*static void HarvestProcedure(void *param)
{
    Harvester *harvester=((Harvester*)param);
    harvester->DoHarvest();
    delete harvester;
}*/

s_bool Harvester::Compare::operator()(const SensorData* s1, const SensorData* s2) const
{
    return s1->timestamp < s2->timestamp;
}

void Harvester::PipesHarvesting(s_double timestampStart, s_double timestampEnd, SensorType sType)
{
    for (s_int32 i=0;i<pipes.size();++i)
    {
        if (i==sType) continue;
        s_bool doHarvest=true;
        SensorData *sd=NULL;
        while (doHarvest)
        {
            sd=pipes[i].ReadMessage<SensorData*>();
            if (sd->timestamp<timestampStart)
                delete sd;
            else if (sd->timestamp<=timestampEnd)
                myHarvest.push_back(sd);
            else{
                nextHarvestData.push_back(sd);
                doHarvest=false;
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

void Harvester::DoHarvest()
{
    s_double timestampStart=masterData->timestamp;
    s_double harvestEpoc_seconds=masterData->rate/1000.f;
    if (masterData->type!=AudioInput)
        timestampStart-=harvestEpoc_seconds;

    InternalBufferHarvesting(timestampStart, timestampStart+harvestEpoc_seconds);
    PipesHarvesting(timestampStart, timestampStart+harvestEpoc_seconds, masterData->type);
    Sort();
}

void Harvester::Harvest(SensorData *_masterData)
{
    masterData=_masterData;
//    Utils::CreateThread((start_routine)HarvestProcedure,this);
    DoHarvest();
}

/*void Harvester::HarvestOnThread(SensorData *masterData)
{

}*/

void Harvester::Sort()
{
    std::sort(myHarvest.begin(), myHarvest.end(), Compare());
}