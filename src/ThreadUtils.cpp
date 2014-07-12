//
//  ThreadUtils.cpp
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#include <ThreadUtils.h>
#include "Logging.h"

#ifdef ANDROID

void thread_exit_handler(int sig)
{
	LOGI("this signal is %d \n", sig);
	pthread_exit(0);
}

void install_signal_handler_for_pthread_kill()
{
	static bool installed = false;
	if (installed)
		return;
	struct sigaction actions;
	memset(&actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0;
	actions.sa_handler = thread_exit_handler;
	int rc = sigaction(SIGUSR1,&actions,NULL);
	installed = true;
}

#endif


scdf::ThreadUtils::ThreadHandle scdf::ThreadUtils::CreateThread(start_routine routine, void *par)
{
#ifdef ANDROID
	install_signal_handler_for_pthread_kill();
#endif
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
#ifdef ANDROID
	int status;
	if ( (status = pthread_kill(handle.tid, SIGUSR1)) != 0)
	{
	    LOGE("Error cancelling thread %ld, error = %d", handle.tid, status);
	}
#else
   pthread_cancel(handle.tid);
#endif
}
