//
//  InitFramework.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "UDPSendersManager.h"
#include "PipesManager.h"
#include "Sensor.h"
#include "SensorsManager.h"

void InitFramework()
{
	scdf::thePipesManager()->InitPipes();
    scdf::thePipesManager()->CreateReturnPipes();
    std::vector<int> udpPorts;
    for (int i=0;i<scdf::NumTypes;++i)
    {
        udpPorts.push_back(9000+i);
    }

    scdf::UDPSendersManager::Instance()->CreateSender(udpPorts, "192.168.1.108");
    scdf::theSensorManager()->CreateAllSensors();
}



