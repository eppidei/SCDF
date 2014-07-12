//
//  ThreadUtils.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include <ThreadUtils.h>

scdf::ThreadUtils::ThreadHandle scdf::ThreadUtils::CreateThread(start_routine routine, void *par)
{
    ThreadHandle t;
    pthread_create(&t.tid, NULL, (start_routine)routine, par);
    return t;
}

void scdf::ThreadUtils::JoinThread(ThreadHandle handle)
{
    void *tmp;
    pthread_join(handle.tid,&tmp);
}

void scdf::ThreadUtils::TerminateThread(ThreadHandle handle)
{
    pthread_cancel(handle.tid);
}
