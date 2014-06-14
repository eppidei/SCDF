//
//  Utils.h
//  SCDF_Test
//
//  Created by Andrea Scuderi on 14/06/14.
//  Copyright (c) 2014 Marco Bertola. All rights reserved.
//

#ifndef SCDF_Test_Utils_h
#define SCDF_Test_Utils_h

#include <pthread.h>
typedef void*(*start_routine)(void*);

namespace scdf
{
    namespace Utils
    {
        pthread_t CreateThread(start_routine routine, void *par)
        {
            pthread_t t;
            pthread_create(&t, NULL, (start_routine)routine, par);
            return t;
        }
        
    }
}


#endif
