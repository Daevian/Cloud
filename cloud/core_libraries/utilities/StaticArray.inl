#ifndef CLOUD_DATASTRUCTURES_STATICARRAY_INLINE
#define CLOUD_DATASTRUCTURES_STATICARRAY_INLINE

#include "DefinesAsserts.h"

template<class Type, CLuint Size>
Cloud::Utils::StaticArray<Type, Size>::StaticArray()
{
}

template<class Type, CLuint Size>
Cloud::Utils::StaticArray<Type, Size>::StaticArray(const StaticArray& staticArray)
{
    for (CLuint i = 0; i < Size; ++i)
    {
        this->m_items[i] = staticArray[i];
    }
}

template<class Type, CLuint Size>
Cloud::Utils::StaticArray<Type, Size>::~StaticArray()
{
}

template<class Type, CLuint Size>
Cloud::Utils::StaticArray<Type, Size>& Cloud::Utils::StaticArray<Type, Size>::operator=(const StaticArray<Type, Size>& staticArray)
{
    for (CLuint i = 0; i < Size; ++i)
    {
        m_items[i] = staticArray[i];
    }

    return *this;
}

template<class Type, CLuint Size>
Type& Cloud::Utils::StaticArray<Type, Size>::operator[](const CLuint& index)
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");
    return m_items[index];
}

template<class Type, CLuint Size>
const Type& Cloud::Utils::StaticArray<Type, Size>::operator[](const CLuint& index) const
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");
    return m_items[index];
}

template<class Type, CLuint Size>
void Cloud::Utils::StaticArray<Type, Size>::Insert(const CLuint& index, const Type& item)
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");

    for (CLuint i = Size - 1; i >= index; --i)
    {
        m_items[i] = m_items[i - 1];
    }

    m_items[index] = item;
}

#endif // CLOUD_DATASTRUCTURES_STATICARRAY_INLINE