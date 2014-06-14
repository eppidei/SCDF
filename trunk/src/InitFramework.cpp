//
//  InitFramework.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 08/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include <pthread.h>
typedef void*(*start_routine)(void*);

static void StartHarvestingProcedure();
void CreatePipes();
void ReadPipes();

pthread_t CreateThread(start_routine routine, void *par)
{
    pthread_t t;
    pthread_create(&t, NULL, (start_routine)routine,par);
    return t;
}

void InitFramework()
{
    CreatePipes();
    CreateThread((start_routine)StartHarvestingProcedure,NULL);
}

static void StartHarvestingProcedure()
{
  /*  while(1)
    {
        ReadPipes();
        SyncSensorsData();
    }
  */ 
}



