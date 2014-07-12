#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include "fcntl.h"
#include "Logging.h"

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

//#define LOG_UDP_SEND
//#define LOG_PIPES_STATUS
//#define LOG_DATA
#define LOG_SENDER_DATA
//#define LOG_TIMESTAMP
//#define LOG_HARVEST_STATUS

#define DEFAULT_UDP_PORT_BASE 7000
#define DEFAULT_IP_ADDRESS "127.0.0.1"
#define OSC_BUFFER_SIZE 4096
#define RETURN_PIPES_STATIC_INIT 50
