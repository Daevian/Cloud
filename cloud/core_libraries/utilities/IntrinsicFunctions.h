#ifndef CLOUD_UTILITY_FUNCTIONS_HEADER
#define CLOUD_UTILITY_FUNCTIONS_HEADER

#include "Windows.h"
#include <stdlib.h>
#include <time.h>
#include "DefinesTypes.h"

inline void* ClMemSet(void* destination, CLint value, CLsize_t size)
{
    return memset(destination, value, size);
}

inline void* ClMemSet(void* destination, const void* source, CLsize_t size)
{
    return memcpy(destination, source, size);
}

inline void* ClMemZero(void* destination, CLsize_t size)
{
    return ClMemSet(destination, 0, size);
}

inline CLtime_t ClTime(CLtime_t* timeValue = 0)
{
    return time(timeValue);
}

inline void ClSRand(CLuint seed)
{
    srand(seed);
}

inline CLint ClRand()
{
    return rand();
}

inline CLfloat ClRandFloat()
{
    return static_cast<CLfloat>(rand()) / static_cast<CLfloat>(RAND_MAX);
}

#endif // CLOUD_UTILITY_FUNCTIONS_HEADER