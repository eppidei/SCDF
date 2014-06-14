//
//  Harvester.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef __SCDF_Test__Harvester__
#define __SCDF_Test__Harvester__

#include <iostream>
#include <vector>
#include "Sensor.h"
namespace scdf{

    class Harvester
    {
        std::vector<SensorData*> nextHarvestData;   //circular buffer?
        std::vector<SensorData*> myHarvest;         //circular buffer?
        SensorData *masterData;
        
        void PipesHarvesting(s_double timestampStart, s_double timestampEnd, SensorType sType);
        void InternalBufferHarvesting(s_double timestampStart, s_double timestampEnd);
        void Sort();
        
        struct Compare {
            s_bool operator()(const SensorData* s1, const SensorData* s2) const;
        };
        
        Harvester(){}
        void DoHarvest();
        static Harvester *_instance;
    public:
        
       // static void HarvestOnThread(SensorData *masterData);
        void Harvest(SensorData *_masterData);
        static Harvester *Instance()
        {
            if (NULL==_instance) _instance=new Harvester();
            return _instance;
        }
    };
}


#endif /* defined(__SCDF_Test__Harvester__) */
