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

    class Harvester
    {
        std::vector<SensorData*> nextHarvestData;   //circular buffer?
        std::vector<SensorData*> myHarvest;         //circular buffer?
        
        struct HarvestInfo{
            std::vector<std::vector<int> > info;
            HarvestInfo() : info(NumTypes) {}
            void CleanUp()
            {
                for(int i=0;i<NumTypes;++i)
                    info[i].clear();
            }
        } myHarvestInfo;
        
        std::vector<SensorData*> bufferHarvest;
        void PipesHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd, SensorType sType);
        void InternalBufferHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd);
        void BuildSensorsDataBuffers(SensorData *masterData);
//        void Sort();
        void NotifyHarvestCompletition();
        void Harvest(SensorData *_masterData);
        void AllocBufferHarvest();
        
        struct Compare {
            s_bool operator()(const SensorData* s1, const SensorData* s2) const;
        };
        
        Harvester();
        static Harvester *_instance;
        SensorType requesterType;
        ThreadUtils::ThreadHandle handle;
    public:
        
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
