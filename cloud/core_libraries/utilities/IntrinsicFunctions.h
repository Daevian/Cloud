#ifndef CLOUD_UTILITY_FUNCTIONS_HEADER
#define CLOUD_UTILITY_FUNCTIONS_HEADER

#include "Windows.h"

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

#endif // CLOUD_UTILITY_FUNCTIONS_HEADER