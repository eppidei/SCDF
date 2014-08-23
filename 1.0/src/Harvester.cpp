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
#include "PipesManager.h"
#include "SensorsManager.h"

using namespace scdf;

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
        SensorData *data=thePipesManager()->ReadFromPipe(harvester->GetType());
        if (NULL!=data)
            harvester->HarvestingProcedure(data);
    }
}

void Harvester::AllocBufferHarvest()
{
    for(int i=0;i<NumTypes;++i)
    {
        SensorData *s=new SensorData((SensorType)i);
        s_int32 bufferSize;
        s_int32 timestampSize;
        s_int32 numChannels=theSensorManager()->GetNumChannels((SensorType)i);
        if (i==AudioInput)
        {
            bufferSize = numChannels*MAX_AUDIO_BUF_LENGTH;
            timestampSize = 2; //start, end
        }else
            bufferSize = timestampSize = numChannels*MAX_SENSOR_BUF_LENGTH;

        s->data= new s_sample[bufferSize];
        s->timestamp= new s_uint64[timestampSize];
        bufferHarvest.push_back(s);
    }
    
}

Harvester::Harvester() : activated(false), requesterType(AudioInput)
{}

void Harvester::Start()
{
    if (activated) return;
    activated=true;
    thePipesManager()->InitPipes();
    theSensorManager()->StartPrecActiveSensors();
    handle=ThreadUtils::CreateThread((start_routine)StartHarvestingProcedure, this);
}

void Harvester::Stop()
{
    if (!activated) return;
    activated=false;
    theSensorManager()->StopAllSensors();
    //Write dummy buffer on master queue to sblock harvester
   // for (int i=0;i<10;++i)
    thePipesManager()->WriteOnPipe(GetType(),thePipesManager()->ReadFromReturnPipe(GetType()));

    ThreadUtils::JoinThread(handle);
    SentDataRecyclingProcedure(&myHarvest);
    SentDataRecyclingProcedure(&nextHarvestData);
}

void Harvester::SetType(SensorType type)
{
    /*bool activate=activated || theSensorManager()->SensorActivated(type);
    if (activated) Stop();
    requesterType=type;
    if (activate) Start();*/
}

/*void Harvester::Sort()
{
    if (myHarvest.size()==0) return;
    std::sort(myHarvest.begin(), myHarvest.end(), Compare());
}*/

void Harvester::PipesHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd, SensorType sType)
{
    for (s_int32 i=0;i<thePipesManager()->NumPipes();++i)
    {
        if (i==sType) continue;
        SensorData *sd=NULL;
        while (1)
        {
            sd=thePipesManager()->ReadFromPipe((SensorType)i);
            if (NULL==sd) break;
            if (sd->timestamp[0]<timestampStart)
            {
#ifdef LOG_HARVEST_STATUS
                LOGD("DELETING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
                if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
                    delete sd;
#ifdef LOG_PIPES_STATUS
                LOGD("Return pipe size of %s: %d\n", SensorTypeString[sd->type].c_str(), returnPipes[sd->type]->GetSize());
#endif
            }
            
            else if (sd->timestamp[0]<=timestampEnd){
#ifdef LOG_HARVEST_STATUS
                printf("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
                sd->timeid=timestampStart;
                myHarvest.push_back(sd);
                myHarvestInfo.info[(int)sd->type].push_back(myHarvest.size()-1);
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
        if (sd->timestamp[0]<timestampStart)
        {
#ifdef LOG_HARVEST_STATUS
            printf("DELETING: Timestamp from %s: %lu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
            if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
                delete sd;
#ifdef LOG_PIPES_STATUS
            LOGD("Return pipe size of %s: %d\n", SensorTypeString[sd->type].c_str(), returnPipes[sd->type]->GetSize());
#endif
            nextHarvestData.erase(nextHarvestData.begin()+i);
        }
        else if (sd->timestamp[0]<=timestampEnd)
        {
#ifdef LOG_HARVEST_STATUS
            printf("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp);
#endif
            myHarvest.push_back(nextHarvestData[i]);
            myHarvestInfo.info[(int)nextHarvestData[i]->type].push_back(myHarvest.size()-1);
            nextHarvestData.erase((nextHarvestData.begin()+i));
        }
    }
}

void Harvester::SentDataRecyclingProcedure(std::vector<SensorData*> *sData)
{
    for (int i=0;i<sData->size();++i)
    {
        if (0==thePipesManager()->WriteOnReturnPipe((*sData)[i]->type,(*sData)[i]))
            delete (*sData)[i];
#ifdef LOG_PIPES_STATUS
        LOGD("Return pipe size of %s: %d\n", SensorTypeString[(*sData)[i]->type].c_str(), returnPipes[(*sData)[i]->type]->GetSize());
#endif
    }
    sData->clear();
}

void Harvester::NotifyHarvestCompletition()
{
    UDPSenderHelperBase *sender=UDPSendersManager::Instance()->GetActiveSender();
    if (NULL==sender) return;
    sender->EventFreeSlot()->Wait();
    sender=UDPSendersManager::Instance()->GetActiveSender();
    if (NULL==sender) return;
    bufferHarvest.swap(sender->senderData);
    sender=UDPSendersManager::Instance()->GetActiveSender();
    if (NULL==sender) return;
    sender->EventCanSend()->Set();
}

void Harvester::BuildSensorsDataBuffers(SensorData *masterData)
{
    if(0==bufferHarvest.size())
       AllocBufferHarvest();
    for (int i=0;i<myHarvestInfo.info.size();++i)   // myHarvestInfo.info.size() = NumTypes
    {
        s_int32 numChannels=theSensorManager()->GetNumChannels((SensorType)i);
        bufferHarvest[i]->num_frames=myHarvestInfo.info[i].size();
        bufferHarvest[i]->num_channels=numChannels;
        bufferHarvest[i]->timeid=masterData->timeid;
        bufferHarvest[i]->rate=theSensorManager()->GetRate((SensorType)i);
        if (AudioInput==i)
        {
            bufferHarvest[i]->num_frames=masterData->num_frames;
            bufferHarvest[i]->timestamp[0]=masterData->timeid;
            bufferHarvest[i]->timestamp[1]=masterData->timestamp[0];
        }
        for(int j=0;j<myHarvestInfo.info[i].size();++j)
        {
            SensorData *currentData=myHarvest[myHarvestInfo.info[i][j]];

            memcpy(bufferHarvest[i]->data+(j*numChannels), currentData->data, currentData->num_frames*currentData->num_channels*sizeof(s_sample));
                
            if (AudioInput==i) continue;
            bufferHarvest[i]->timestamp[j]=currentData->timestamp[0];
        }
    }
}

void Harvester::HarvestingProcedure(SensorData *masterData)
{
    Harvest(masterData);
#ifdef LOG_DATA
    for (int i = 0; i< masterData->num_samples; i ++) {
        printf("Harvested data %d from %s: %.4f\n",i,SensorTypeString[masterData->type].c_str(), ((s_sample*)masterData->data)[i]);
    }
#endif
    BuildSensorsDataBuffers(masterData);
    SentDataRecyclingProcedure(&myHarvest);
    NotifyHarvestCompletition();
}

void Harvester::Harvest(SensorData *masterData)
{
#ifdef LOG_TIMESTAMP
    printf("Master sensor: %s; Harvesting starts: %llu; Harvesting ends: %llu; Harvesting ms interval: %llu;\n",SensorTypeString[masterData->type].c_str(),masterData->timeid,masterData->timestamp,getUptimeInMilliseconds(masterData->timestamp-masterData->timeid));
#endif
    myHarvestInfo.CleanUp();
    InternalBufferHarvesting(masterData->timeid, masterData->timestamp[0]);
    PipesHarvesting(masterData->timeid, masterData->timestamp[0], masterData->type);
    //Sort();
    myHarvest.push_back(masterData);
    //myHarvest.insert(myHarvest.begin(), masterData);
    myHarvestInfo.info[masterData->type].push_back(myHarvest.size()-1);
}
