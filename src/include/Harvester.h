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

namespace scdf{

    class Harvester
    {
        std::vector<SensorData*> nextHarvestData;   //circular buffer?
        std::vector<SensorData*> myHarvest;         //circular buffer?
        SensorData *masterData;
        
        void PipesHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd, SensorType sType);
        void InternalBufferHarvesting(s_uint64 timestampStart, s_uint64 timestampEnd);
        void SetupPipes();
        void Sort();
        void NotifyHarvestCompletition();
        void Harvest(SensorData *_masterData);
        
        struct Compare {
            s_bool operator()(const SensorData* s1, const SensorData* s2) const;
        };
        
        Harvester();
        static Harvester *_instance;
        SensorType requesterType;
    public:
        bool activated;
        SensorType GetType() { return requesterType; }
        void SetType(SensorType type);
        void HarvestingProcedure(SensorData *_masterData);
        static Harvester *Instance()
        {
            if (NULL==_instance) _instance=new Harvester();
            return _instance;
        }
    };
}


#endif /* defined(__SCDF_Test__Harvester__) */
