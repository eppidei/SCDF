//
//  InitFramework.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include "Harvester.h"
#include "UDPSendersManager.h"
#include <vector>
void CreatePipes();

void InitFramework()
{
    CreatePipes();
    std::vector<int> udpPorts;
    udpPorts.push_back(7474);
    scdf::UDPSendersManager::Instance()->CreateSender(udpPorts, "127.0.0.1");
    scdf::Harvester::Instance();
}



