#ifndef _TYPEDEFINITION_H
#define _TYPEDEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iostream>
#include <memory>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include "fcntl.h"
#include "Logging.h"
#include <stdexcept>

typedef bool    s_bool;
typedef int8_t  s_int8;
typedef char    s_char;
typedef int16_t   s_int16;
typedef int32_t   s_int32;
typedef int64_t   s_int64;
typedef long    s_long;

typedef unsigned char 	s_byte;
typedef uint16_t 		s_uint16;
typedef uint32_t   		s_uint32;
typedef uint64_t 		s_uint64;
typedef unsigned long    s_ulong;

typedef float   s_float;
typedef double  s_double;
typedef s_float s_sample;

#define CALLBACK

#define LOG_UDP_SEND
//#define LOG_PIPES_STATUS
//#define LOG_DATA
//#define LOG_SENDER_DATA
//#define LOG_TIMESTAMP
//#define LOG_HARVEST_STATUS
//#define LOG_SEM

#define DEFAULT_UDP_PORT_BASE 9000
#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define RETURN_PIPES_STATIC_INIT 50

#define MAX_SAMP_FREQ 48000
#define MAX_SENSOR_RATE 100
#define MAX_AUDIO_BUF_LENGTH 4096

#define MAX_SENSOR_BUF_LENGTH 9

#define DEFAULT_SENORS_STANDARD_RATE 100
#define DEFAULT_AUDIO_BUFFER_SIZE 512

#ifdef PLATF_IOS
#define SCDF_API __attribute__((visibility("default")))
#else
#define SCDF_API
#endif

#endif


