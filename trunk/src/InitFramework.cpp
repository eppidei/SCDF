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
	//scdf::thePipesManager()->InitPipes();
    scdf::thePipesManager()->CreateReturnPipes();
    scdf::theSensorManager()->CreateAllSensors();
    scdf::Harvester::Instance();
    scdf::Harvester::Instance()->SetType(scdf::AudioInput);
    scdf::Harvester::Instance()->Start();
    
}



