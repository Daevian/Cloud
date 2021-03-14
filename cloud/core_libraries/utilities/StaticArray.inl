#ifndef CLOUD_DATASTRUCTURES_STATICARRAY_INLINE
#define CLOUD_DATASTRUCTURES_STATICARRAY_INLINE

#include "DefinesAsserts.h"

template<class Type, uint Size>
#pragma warning(suppress: 26495)
Cloud::Utils::StaticArray<Type, Size>::StaticArray()
{
}

template<class Type, uint Size>
Cloud::Utils::StaticArray<Type, Size>::StaticArray(const StaticArray& staticArray)
{
    for (uint i = 0; i < Size; ++i)
    {
        this->m_items[i] = staticArray[i];
    }
}

template<class Type, uint Size>
Cloud::Utils::StaticArray<Type, Size>::~StaticArray()
{
}

template<class Type, uint Size>
Cloud::Utils::StaticArray<Type, Size>& Cloud::Utils::StaticArray<Type, Size>::operator=(const StaticArray<Type, Size>& staticArray)
{
    for (uint i = 0; i < Size; ++i)
    {
        m_items[i] = staticArray[i];
    }

    return *this;
}

template<class Type, uint Size>
Type& Cloud::Utils::StaticArray<Type, Size>::operator[](const uint& index)
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");
    return m_items[index];
}

template<class Type, uint Size>
const Type& Cloud::Utils::StaticArray<Type, Size>::operator[](const uint& index) const
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");
    return m_items[index];
}

template<class Type, uint Size>
void Cloud::Utils::StaticArray<Type, Size>::Insert(const uint& index, const Type& item)
{
    CL_ASSERT(index >= 0 && index < Size, "Index is out of bounds!");

    for (uint i = Size - 1; i >= index; --i)
    {
        m_items[i] = m_items[i - 1];
    }

    m_items[index] = item;
}

#endif // CLOUD_DATASTRUCTURES_STATICARRAY_INLINE