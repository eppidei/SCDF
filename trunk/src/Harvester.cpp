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

s_uint64 now_ns(void); // definition in sensorstandardimplandroid.cpp

static void StartHarvestingProcedure(void *param)
{
    Harvester *harvester=((Harvester*)param);
    while(harvester->activated)
    {
    	//s_uint64 start = now_ns();
    	//LOGD("Read master pipe");
        SensorData *data=thePipesManager()->ReadFromPipe(harvester->GetType());
        //s_uint64 elap = now_ns() - start;
        //LOGD("Read from master pipe done. %f ms",(elap/1000000.0));
        if (NULL!=data)
            harvester->HarvestingProcedure(data);
    }
}

std::vector<SensorData*> *Harvester::AllocBufferHarvest()
{
    std::vector<SensorData*> *buffer=new std::vector<SensorData*>();
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
        buffer->push_back(s);
    }
    return buffer;
    
}

Harvester::Harvester() : activated(false), requesterType(AudioInput)
{}

void Harvester::Start()
{
    if (activated) return;
    activated=true;
    thePipesManager()->InitPipes();
    thePipesManager()->InitReturnPipes();
    theSensorManager()->StartPrecActiveSensors();
    handle=ThreadUtils::CreateThread((start_routine)StartHarvestingProcedure, this);
}

void Harvester::Stop()
{
    if (!activated) return;
    activated=false;
    theSensorManager()->StopAllSensors();
    //Write dummy buffer on master queue to unlock harvester
    thePipesManager()->WriteOnPipe(GetType(),thePipesManager()->ReadFromReturnPipe(GetType()));
    ThreadUtils::JoinThread(handle);
    SentDataRecyclingProcedure(&harvestData);
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
        if (i==sType) continue; // don't harvest the master pipe, already done
        SensorData *sd=NULL;
        while (1)
        {
            sd=thePipesManager()->ReadFromPipe((SensorType)i);
            if (NULL==sd) break;
            if (sd->timestamp[0]<timestampStart)
            {
#ifdef LOG_HARVEST_STATUS
                LOGD("DELETING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
#endif
                if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
                    delete sd;
#ifdef LOG_PIPES_STATUS
                LOGD("PipesHarvesting. %s pipe size: %d\n", SensorTypeString[sd->type].c_str(), (*(thePipesManager()->GetReturnPipes()))[sd->type]->GetSize());
#endif
            }
            
            else if (sd->timestamp[0]<=timestampEnd){
#ifdef LOG_HARVEST_STATUS
                LOGD("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
#endif
                sd->timeid=timestampStart;
                harvestData.push_back(sd);
                myHarvestInfo.info[(int)sd->type].push_back(harvestData.size()-1);
            }
            else{
#ifdef LOG_HARVEST_STATUS
                LOGD("ADDING TO NEXT: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
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
            LOGD("DELETING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
#endif
            if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
                delete sd;
#ifdef LOG_PIPES_STATUS
            LOGD("InternalBufferHarvesting. %s pipe size: %d\n", SensorTypeString[sd->type].c_str(), (*(thePipesManager()->GetReturnPipes()))[sd->type]->GetSize());
#endif
            nextHarvestData.erase(nextHarvestData.begin()+i);
        }
        else if (sd->timestamp[0]<=timestampEnd)
        {
#ifdef LOG_HARVEST_STATUS
            LOGD("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
#endif
            harvestData.push_back(nextHarvestData[i]);
            myHarvestInfo.info[(int)nextHarvestData[i]->type].push_back(harvestData.size()-1);
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
        LOGD("SentDataRecyclingProcedure. Size of %s RETURN pipe: %d\n", SensorTypeString[(*sData)[i]->type].c_str(), (*(thePipesManager()->GetReturnPipes()))[(*sData)[i]->type]->GetSize());
#endif
    }
    sData->clear();
}

void Harvester::SendingQueuePushBuffer(std::vector<SensorData*> *buffer)
{
    UDPSenderHelperBase *sender=UDPSendersManager::Instance()->GetSender();
    if (NULL==sender) return;
    syncDataQueue.push(buffer);
#ifdef LOG_SEM
    LOGD("Sender data queue size :%d\n",syncDataQueue.size());
#endif
    harvestReady.Set();
}

void Harvester::WaitForHarvest()
{
    harvestReady.Wait();
}

std::vector<SensorData*> *Harvester::BuildSensorsDataBuffers(SensorData *masterData)
{
    std::vector<SensorData*> *bufferHarvest=AllocBufferHarvest();
    for (int i=0;i<myHarvestInfo.info.size();++i)   // myHarvestInfo.info.size() = NumTypes
    {
        s_int32 numChannels=theSensorManager()->GetNumChannels((SensorType)i);
        (*bufferHarvest)[i]->num_frames=myHarvestInfo.info[i].size();
        (*bufferHarvest)[i]->num_channels=numChannels;
        (*bufferHarvest)[i]->timeid=masterData->timeid;
        (*bufferHarvest)[i]->rate=theSensorManager()->GetRate((SensorType)i);
        if (AudioInput==i)
        {
            (*bufferHarvest)[i]->num_frames=masterData->num_frames;
            (*bufferHarvest)[i]->timestamp[0]=masterData->timeid;
            (*bufferHarvest)[i]->timestamp[1]=masterData->timestamp[0];
        }
        for(int j=0;j<myHarvestInfo.info[i].size();++j)
        {
            SensorData *currentData=harvestData[myHarvestInfo.info[i][j]];

            memcpy((*bufferHarvest)[i]->data+(j*numChannels), currentData->data, currentData->num_frames*currentData->num_channels*sizeof(s_sample));
                
            if (AudioInput==i) continue;
            (*bufferHarvest)[i]->timestamp[j]=currentData->timestamp[0];
        }
    }
    return bufferHarvest;
}

void Harvester::HarvestingProcedure(SensorData *masterData)
{
    Harvest(masterData);
#ifdef LOG_DATA
    for (int i = 0; i< masterData->num_samples; i ++) {
        printf("Harvested data %d from %s: %.4f\n",i,SensorTypeString[masterData->type].c_str(), ((s_sample*)masterData->data)[i]);
    }
#endif
    std::vector<SensorData*> *b=BuildSensorsDataBuffers(masterData);
    SentDataRecyclingProcedure(&harvestData);
    SendingQueuePushBuffer(b);
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
    harvestData.push_back(masterData);
    myHarvestInfo.info[masterData->type].push_back(harvestData.size()-1);
}
