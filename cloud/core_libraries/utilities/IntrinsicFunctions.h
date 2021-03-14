#ifndef CLOUD_UTILITY_FUNCTIONS_HEADER
#define CLOUD_UTILITY_FUNCTIONS_HEADER

#include "Windows.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "DefinesTypes.h"

inline void* ClMemSet(void* destination, int value, size_t size)
{
    return memset(destination, value, size);
}

inline void* ClMemCopy(void* destination, const void* source, size_t size)
{
    return memcpy(destination, source, size);
}

inline void* ClMemZero(void* destination, size_t size)
{
    return ClMemSet(destination, 0, size);
}

inline size_t ClStrlen(const t_char* str)
{
    return strlen(str);
}

inline time_t ClTime(time_t* timeValue = 0)
{
    return time(timeValue);
}

inline void ClSRand(uint seed)
{
    srand(seed);
}

inline int ClRand()
{
    return rand();
}

inline float ClRandFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

#endif // CLOUD_UTILITY_FUNCTIONS_HEADER