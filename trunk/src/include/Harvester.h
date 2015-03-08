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
    
    
    class HarvesterListener
    {
    public:
        virtual void OnHarvesterBufferReady(std::vector<SensorData*> *buffer) = 0;
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
        HarvesterListener *harversterListener;
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
    public:
        int GetUpdateInterval(){return uptateIntervalMs;}
        s_bool IsAudioSyncActive();
        void SetUpdateIntervalWithNoAudioSynch(int _uptateIntervalMs);
        void SetHarvesterListener(HarvesterListener *listner){harversterListener = listner;}
        void SendingQueuePushBuffer(std::vector<SensorData*> *buffer);
        void WaitForHarvest();
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
        StopRestartMachine() : startMachine(Harvester::Instance()->activated)
        {
            Harvester::Instance()->Stop();
        }
        ~StopRestartMachine()
        {
            if (startMachine)
                Harvester::Instance()->Start();
        }
    };
}


#endif /* defined(__SCDF_Test__Harvester__) */
