//
//  ThreadUtils.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef SCDF_Test_Utils_h
#define SCDF_Test_Utils_h

#include <pthread.h>
#import "dispatch/semaphore.h"
#include "TypeDefinitions.h"
typedef void*(*start_routine)(void*);

namespace scdf
{
    namespace ThreadUtils
    {
        typedef pthread_mutex_t CUSTOM_MUTEX;
        pthread_t CreateThread(start_routine routine, void *par);
        class CustomMutex {
            
            CustomMutex(const CustomMutex &refCustomMutex);
			CustomMutex &operator=(const CustomMutex &refCustomMutex);
            
			CUSTOM_MUTEX cMutex;
		public:
			CustomMutex() {
                pthread_mutexattr_t    attr={0};
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(&cMutex, &attr);
                pthread_mutexattr_destroy(&attr);
			};
            
			~CustomMutex() {
				pthread_mutex_destroy(&cMutex);
			};
            
			void Lock() {
				pthread_mutex_lock(&cMutex);
			};
            
			void Unlock() {
				pthread_mutex_unlock(&cMutex);
			};
		};
        class CustomSemaphore
        {
            dispatch_semaphore_t sem;
        public:
            CustomSemaphore()
            {
                sem=dispatch_semaphore_create(0);
            }
            CustomSemaphore(s_int32 initValue)
            {
                sem=dispatch_semaphore_create(initValue);
            }
            void Set()
            {
                dispatch_semaphore_signal(sem);
            }
            bool Wait()
            {
                dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
                return true;
            }
        };
        class AutoLock {
            
			// make copy constructor and assignment operator inaccessible
            
			AutoLock(const AutoLock &refAutoLock);
			AutoLock &operator=(const AutoLock &refAutoLock);
            
		protected:
			CustomMutex * pLock;
            
		public:
			AutoLock(CustomMutex * _plock)
			{
				pLock = _plock;
				pLock->Lock();
			};
            
			~AutoLock() {
				pLock->Unlock();
			};
		};
    }
}
#endif
