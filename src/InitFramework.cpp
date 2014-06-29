//
//  InitFramework.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "UDPSendersManager.h"
#include "Sensor.h"

void CreatePipes();
void CreateReturnPipes();

void InitFramework()
{
    CreatePipes();
    CreateReturnPipes();
    std::vector<int> udpPorts;
    for (int i=0;i<scdf::NumTypes;++i)
    {
        udpPorts.push_back(9000+i);
    }

    scdf::UDPSendersManager::Instance()->CreateSender(udpPorts, "192.168.1.107");
    scdf::Harvester::Instance();
}



