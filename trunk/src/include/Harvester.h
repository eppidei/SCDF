//
//  Harvester.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__Harvester__
#define __SCDF_Test__Harvester__

#include <algorithm>
#include "Sensor.h"
#include "ThreadUtils.h"

namespace scdf{
    
    class HarvesterListener;
    
    class HarvesterListenerContainer
    {
        std::map <SensorType, int> listenersRefCount;
        std::map <HarvesterListener*, std::vector<SensorType>> listenersMap;
        scdf::ThreadUtils::CustomMutex controlUnitLock;
        
        int GetTotalRefCount();
        void CheckRefCountForToStartAndStopHarvester();
        
    public:
        HarvesterListenerContainer();
        
        void Attach(HarvesterListener* _listener,std::vector<SensorType> _typeList );
        void Detach(HarvesterListener* _listener);
        
        bool IsAny();
        
        void OnHarvesterBufferReady(std::vector<SensorData*> *buffer);
    };
    
    
  


    class Harvester
    {
        friend class UDPSenderHelperBase;
        std::vector<SensorData*> nextHarvestData;   //circular buffer?
        std::vector<SensorData*> harvestData;         //circular buffer?

        std::queue<std::vector<SensorData*>* > syncDataQueue;
        
        static Harvester *_instance;
        SensorType requesterType;
        ThreadUtils::ThreadHandle handle;
        ThreadUtils::CustomSemaphore harvestReady;
        //HarvesterListener *harversterListener;
        int uptateIntervalMs;
        
        struct HarvestInfo{
            std::vector<std::vector<int> > info;
            HarvestInfo() : info(NumTypes) {}
            void CleanUp()
            {
                for(int i=0;i<NumTypes;++i)
                    info[i].clear();
            }
        } myHarvestInfo;
        
        void PipesHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd, SensorType sType);
        void InternalBufferHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd);
        std::vector<SensorData*> *BuildSensorsDataBuffers(SensorData *masterData);
        void Harvest(SensorData *_masterData);
        std::vector<SensorData*> *AllocBufferHarvest();
        
        struct Compare {
            s_bool operator()(const SensorData* s1, const SensorData* s2) const;
        };
        
        Harvester();
        void CheckMaxQueueDim();
        
        HarvesterListenerContainer listenersContainer;
    public:
        HarvesterListenerContainer *GetListeners(){return &listenersContainer;}
        
        int GetUpdateInterval(){return uptateIntervalMs;}
        s_bool IsAudioSyncActive();
        void SetUpdateIntervalWithNoAudioSynch(int _uptateIntervalMs);
        //void SetHarvesterListener(HarvesterListener *listner){harversterListener = listner;}
        void SendingQueuePushBuffer(std::vector<SensorData*> *buffer);
        void WaitForHarvest();
        bool IsActive(){return activated;}
        bool activated;
        SensorType GetType() { return requesterType; }
        void SetType(SensorType type);
        void HarvestingProcedure(SensorData *_masterData);
        void Start();
        void Stop();
        static void SentDataRecyclingProcedure(std::vector<SensorData*> *sData);
        static Harvester *Instance()
        {
            if (NULL==_instance) _instance=new Harvester();
            return _instance;
        }
    };
    
    class StopRestartMachine
    {
        bool startMachine;
    public:
        StopRestartMachine() : startMachine(Harvester::Instance()->IsActive())
        {
            Harvester::Instance()->Stop();
        }
        virtual ~StopRestartMachine()
        {
            if (startMachine)
                Harvester::Instance()->Start();
        }
    };
    class StopRestartMachineAs : public StopRestartMachine
    {
        SensorType type;
    public:
        StopRestartMachineAs(SensorType newType) : StopRestartMachine(), type(newType)
        {
        }
        ~StopRestartMachineAs()
        {
            Harvester::Instance()->SetType(type);
        }
    };
}


#endif /* defined(__SCDF_Test__Harvester__) */
