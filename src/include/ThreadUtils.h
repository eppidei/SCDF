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
#ifndef ANDROID
#import "dispatch/semaphore.h"
#else
#include <semaphore.h>
#endif
#include "TypeDefinitions.h"

typedef void*(*start_routine)(void*);

namespace scdf
{
    namespace ThreadUtils
    {
        typedef pthread_mutex_t CUSTOM_MUTEX;
        struct ThreadHandle{
            pthread_t tid;
        };
        ThreadHandle CreateThread(start_routine routine, void *par);
        void JoinThread(ThreadHandle handle);
        void TerminateThread(ThreadHandle handle);
        class CustomMutex {
            
            CustomMutex(const CustomMutex &refCustomMutex);
			CustomMutex &operator=(const CustomMutex &refCustomMutex);
            
			CUSTOM_MUTEX cMutex;
		public:
			CustomMutex() {
                pthread_mutexattr_t    attr={0};
                pthread_mutexattr_init(&attr);
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
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
            int initValue;
#ifndef ANDROID
            dispatch_semaphore_t sem;
            void Init(s_int32 _initValue)
            {
                initValue=_initValue;
                sem=dispatch_semaphore_create(_initValue);
            }
        public:
            CustomSemaphore()
            {
                Init(0);
            }
            CustomSemaphore(s_int32 _initValue)
            {
                Init(_initValue);
            }
            void Set()
            {
                initValue++;
#ifdef LOG_SEM
                LOGD("Semaphore dim: %d\n", initValue);
#endif
                dispatch_semaphore_signal(sem);
            }
            bool Wait()
            {
                initValue--;
#ifdef LOG_SEM
                LOGD("Semaphore dim: %d\n", initValue);
#endif
                dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
                return true;
            }
#else // android

        sem_t sem;

            void Init(s_int32 _initValue)
            {
                initValue=_initValue;
                sem_init(&sem,0,_initValue);
            }
        public:
            CustomSemaphore()
            {
                Init(0);
            }
            CustomSemaphore(s_int32 _initValue)
            {
            	
            	Init(_initValue);
            }
            void Set()
            {
            	sem_post(&sem);
            	//dispatch_semaphore_signal(sem);
            }
            bool Wait()
            {
            	sem_wait(&sem);
            	//dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
                return true;
            }
            ~CustomSemaphore()
            {
            	sem_destroy(&sem);
            }

#endif

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
