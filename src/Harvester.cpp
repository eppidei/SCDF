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
#include "ScdfSensorAPI.h"

#include <chrono>
#include <thread>


s_uint64 now_ns();


using namespace scdf;



Harvester *Harvester::_instance=NULL;
s_bool Harvester::Compare::operator()(const SensorData* s1, const SensorData* s2) const
{
    return s1->timestamp < s2->timestamp;
}

static void StartHarvestingProcedure(void *param)
{
    Harvester *harvester=((Harvester*)param);
    s_uint64 startTime = now_ns();
   
    while(harvester->activated)
    {
        
    	s_uint64 start = now_ns();
//    	LOGD("Read master pipe");
        SensorData *data = NULL;
        
        if(harvester->IsAudioSyncActive())
        {
            //LOGD("StartHarvestingProcedure with Audio Synch ON\n")
            
            data=thePipesManager()->ReadFromPipe(harvester->GetType());
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(harvester->GetUpdateInterval()));
            
            //LOGD("StartHarvestingProcedure with Audio Synch OFF\n")
           
            data=thePipesManager()->ReadFromReturnPipe(AudioInput);
            
            s_uint64 actualTime = now_ns();
            
            data->timestamp[0] = startTime;
            data->timestamp[1] = actualTime;
            data->timeid = startTime;
            
            startTime = actualTime;
        }
//        s_uint64 elap = now_ns() - start;
        
//        LOGD("Read from master pipe done. %f ms",(elap/1000000.0));
        
        if (NULL!=data)
            harvester->HarvestingProcedure(data);
        
    }
}

std::vector<SensorData*> *Harvester::AllocBufferHarvest()
{
    
    std::vector<SensorData*> *bufferTemp;
    
    if (/*harversterListener!=NULL*/GetListeners()->IsAny())
    {
        static std::vector<SensorData*> *buffer=new std::vector<SensorData*>();
        bufferTemp=buffer;
    }
    else
    {
        std::vector<SensorData*> *buffer=new std::vector<SensorData*>();
        bufferTemp=buffer;
    }
    
    if (bufferTemp->size()==0)
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
            bufferTemp->push_back(s);
        }
    }
    return bufferTemp;

    
}

Harvester::Harvester() : activated(false), requesterType(AudioInput),/* harversterListener(NULL),*/uptateIntervalMs(40)
{
    
}

void Harvester::CheckMaxQueueDim()
{
    static const int MaxQueueDim = 100;
    if (syncDataQueue.size()<MaxQueueDim) return;
    
    harvestReady.Wait();
    std::vector<SensorData*> *buffer=syncDataQueue.front();
    syncDataQueue.pop();
#ifdef LOG_SEM
    LOGD("Sender data queue size :%d\n",syncDataQueue.size());
#endif
    if (NULL==buffer) return;
    for(int i=0;i<buffer->size();++i)
        delete (*buffer)[i];
    delete buffer;
}


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
    //if(IsAudioSyncActive())
    thePipesManager()->WriteOnPipe(GetType(),thePipesManager()->ReadFromReturnPipe(GetType()));
    ThreadUtils::JoinThread(handle);
    SentDataRecyclingProcedure(&harvestData);
    SentDataRecyclingProcedure(&nextHarvestData);
}

void Harvester::SetType(SensorType type)
{
    requesterType=type;
    //bool activate=activated || theSensorManager()->SensorActivated(type);
    //if (activated) Stop();
    //requesterType=type;
    //if (activate) Start();
}

/*void Harvester::Sort()
{
    if (myHarvest.size()==0) return;
    std::sort(myHarvest.begin(), myHarvest.end(), Compare());
}*/

//#define THROW_AWAY_OLD_VALUES


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

		#ifdef THROW_AWAY_OLD_VALUES
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
            else
         #endif
            {
		    	if (sd->timestamp[0]<=timestampEnd){
					#ifdef LOG_HARVEST_STATUS
                	LOGD("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
					#endif
                	sd->timeid=timestampStart;
                	harvestData.push_back(sd);
                	myHarvestInfo.info[(int)sd->type].push_back(harvestData.size()-1);
            	}
            	else{
					#ifdef LOG_HARVEST_STATUS
            		//if (sd->type==Proximity)
            		LOGD("ADDING TO NEXT: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
					#endif
            		nextHarvestData.push_back(sd);
                	break;
            	}
            }
        }
    }
}

void Harvester::InternalBufferHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd)
{
    SensorData *sd=NULL;

    for (s_int32 i=0;i<nextHarvestData.size();/*++i*/)
    {
        sd=nextHarvestData[i];

#ifdef THROW_AWAY_OLD_VALUES
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
        else
#endif
        if (sd->timestamp[0]<=timestampEnd)
        {
			#ifdef LOG_HARVEST_STATUS
            LOGD("ADDING: Timestamp from %s: %llu\n", SensorTypeString[sd->type].c_str(), sd->timestamp[0]);
			#endif
            harvestData.push_back(nextHarvestData[i]);
            myHarvestInfo.info[(int)nextHarvestData[i]->type].push_back(harvestData.size()-1);
            nextHarvestData.erase((nextHarvestData.begin()+i));

            // TODO: understand... should I send the collected sensor data back to the sensor?
            // it seems to me not, since the buffer is now harvested but must be sent... or not?!?
            // if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
            //	delete sd;
        }
        else {
        	// the sensor data still belongs to the future. keep it into the nextHarvestData for next round
        	//if (0==thePipesManager()->WriteOnReturnPipe(sd->type,sd))
			//  delete sd;
        	//nextHarvestData.erase(nextHarvestData.begin()+i);
        	i++;
        }

    }
}

void Harvester::SentDataRecyclingProcedure(std::vector<SensorData*> *sData)
{
    for (int i=0;i<sData->size();++i)
    {
        //assert((*sData)[i]->data!=NULL);
        
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
    //LOGD("UDP SENDER - Harvester SendingQueuePushBUffer");
    
    UDPSenderHelperBase *sender=UDPSendersManager::Instance()->GetSender();
    if (NULL==sender) return;
    
    CheckMaxQueueDim();
    
    syncDataQueue.push(buffer);
#ifdef LOG_SEM
    LOGD("Sender data queue size :%d\n",syncDataQueue.size());
#endif
    
    //LOGD("UDP SENDER - Harvester setting harvestReady...");
    harvestReady.Set();
}

void Harvester::WaitForHarvest()
{
	//LOGD("UDP SENDER - Harvester::WaitForHarwest()");
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
    
    if (/*NULL!=harversterListener*/ GetListeners()->IsAny())
        GetListeners()->OnHarvesterBufferReady(b);
    else
        SendingQueuePushBuffer(b);

//    Harvest(masterData);
//#ifdef LOG_DATA
//    for (int i = 0; i< masterData->num_samples; i ++) {
//        printf("Harvested data %d from %s: %.4f\n",i,SensorTypeString[masterData->type].c_str(), ((s_sample*)masterData->data)[i]);
//    }
//#endif
//    std::vector<SensorData*> *b=BuildSensorsDataBuffers(masterData);
//    SentDataRecyclingProcedure(&harvestData);
//    if(harversterListener)
//        harversterListener->OnHarvesterBufferReady(b);
//    SendingQueuePushBuffer(b);
}

void Harvester::Harvest(SensorData *masterData)
{
#ifdef LOG_TIMESTAMP
    printf("Master sensor: %s; Harvesting starts: %llu; Harvesting ends: %llu; Harvesting ms interval: %llu;\n",SensorTypeString[masterData->type].c_str(),masterData->timeid,masterData->timestamp,getUptimeInMilliseconds(masterData->timestamp-masterData->timeid));
#endif
    //harversterListener = NULL;
    myHarvestInfo.CleanUp();
    InternalBufferHarvesting(masterData->timeid, masterData->timestamp[0]);
    PipesHarvesting(masterData->timeid, masterData->timestamp[0], masterData->type);
    //Sort();
    harvestData.push_back(masterData);
    myHarvestInfo.info[masterData->type].push_back(harvestData.size()-1);
}

void Harvester::SetUpdateIntervalWithNoAudioSynch(int _uptateIntervalMs)
{
    assert(_uptateIntervalMs);
    uptateIntervalMs = _uptateIntervalMs;
}


s_bool Harvester::IsAudioSyncActive()
{
    return scdf::theSensorAPI()->IsSensorActive(AudioInput);
}


HarvesterListenerContainer::HarvesterListenerContainer()
{
     for(int i=0;i<NumTypes;++i)
     {
         listenersRefCount.insert(std::pair<SensorType,int>((SensorType)i,0));
     }
}

void HarvesterListenerContainer::OnHarvesterBufferReady(std::vector<SensorData*> *buffer)
{
    scdf::ThreadUtils::AutoLock kk(&controlUnitLock);
    for(auto it = listenersMap.begin(); it != listenersMap.end(); it++)
        it->first->OnHarvesterBufferReady(buffer);

}

void HarvesterListenerContainer::Attach(HarvesterListener* _listener, std::vector<SensorType> _typeList  )
{
    if(NULL==_listener) return;
    if (0==_typeList.size()) return;
    
    {
    scdf::ThreadUtils::AutoLock kk(&controlUnitLock);
    listenersMap[_listener]=_typeList;
    
    for(int i = 0; i<_typeList.size(); i++)
    {
        bool success = scdf::theSensorManager()->StartSensor(_typeList[i]);

        if (!success) {
        	LOGE("HarvesterListenerContainer::Attach error: could not start sensor!");
        	assert(false);
        	continue;
        }

        listenersRefCount[_typeList[i]]++;
        int numFrames=scdf::theSensorManager()->GetNumFramesPerCallback(_typeList[i])*scdf::theSensorManager()->GetNumChannels(_typeList[i]);
        _listener->Init(numFrames, scdf::theSensorManager()->GetRate(_typeList[i]));
    }
    }
    CheckRefCountForToStartAndStopHarvester();
}

void HarvesterListenerContainer::Detach(HarvesterListener* _listener )
{
    if(NULL==_listener) return;
   
    {
    scdf::ThreadUtils::AutoLock kk(&controlUnitLock);
    auto it = listenersMap.find(_listener);
    if (it==listenersMap.end()) return;
    
    for(int i = 0; i<it->second.size(); i++)
    {
        listenersRefCount[it->second[i]]--;
        _listener->Release();
        if(0==listenersRefCount[it->second[i]])
            scdf::theSensorManager()->StopSensor(it->second[i]);
    }
   
    listenersMap.erase(_listener);
    }
    CheckRefCountForToStartAndStopHarvester();
}

bool HarvesterListenerContainer::IsAny()
{
    if(GetTotalRefCount()>0)
        return true;
    
    return false;
}

int HarvesterListenerContainer::GetTotalRefCount()
{
    int count = 0;
    for(auto it = listenersMap.begin(); it != listenersMap.end(); it++)
        count += it->second.size();
    
    return count;
}

void HarvesterListenerContainer::CheckRefCountForToStartAndStopHarvester()
{
    if (!GetTotalRefCount()) {
         if(Harvester::Instance()->IsActive())
            scdf::Harvester::Instance()->Stop();
    } else
    {
        if(!Harvester::Instance()->IsActive())
            scdf::Harvester::Instance()->Start();
    }
    
}
