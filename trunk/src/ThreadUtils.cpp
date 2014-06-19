//
//  ThreadUtils.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include <ThreadUtils.h>

pthread_t scdf::ThreadUtils::CreateThread(start_routine routine, void *par)
{
    pthread_t t;
    pthread_create(&t, NULL, (start_routine)routine, par);
    return t;
}
 