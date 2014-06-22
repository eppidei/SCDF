#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <assert.h>

using namespace std;

typedef bool    s_bool;

typedef char    s_int8;
typedef char    s_char;
typedef short   s_int16;
typedef int     s_int32;
typedef long    s_int64;

typedef unsigned char  s_byte;
typedef unsigned short s_uint16;
typedef unsigned int   s_uint32;
typedef unsigned long  s_uint64;

typedef float   s_float;
typedef double  s_double;
typedef s_float s_sample;

#define CALLBACK

//#define TEST_PRINT_DATA

#define TEST_UDP_PORT_BASE 7000
#define TEST_IP_ADDRESS "127.0.0.1"
#define OSC_BUFFER_SIZE 4096
