#ifndef TINY_TYPES
#define TINY_TYPES


typedef unsigned char uchar;
typedef unsigned char byte;

typedef unsigned int uint;
typedef unsigned short ushort;

#ifdef TINY_TYPES_PREFER_XDATA
    #define WISE_XDATA xdata
#else
    #define WISE_XDATA
#endif

#endif