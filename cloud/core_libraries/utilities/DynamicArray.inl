#ifndef CLOUD_DATASTRUCTURES_DYNAMICARRAY_INLINE
#define CLOUD_DATASTRUCTURES_DYNAMICARRAY_INLINE

#include "DefinesMacros.h"
#include "DefinesAsserts.h"

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray()
:m_items(0)
,m_last(0)
,m_count(0)
,m_maxCount(0)
,m_growSize(1)
{
    Init(0, 1);
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray(const CLint& startSize, const CLint& growSize)
:m_items(0)
,m_last(0)
,m_count(0)
,m_maxCount(startSize)
,m_growSize(growSize)
{
    Init(startSize, growSize);
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray(const DynamicArray& dynamicArray)
:m_items(0)
{
    *this = dynamicArray;
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::~DynamicArray()
{
    m_last = 0;
    SAFE_DELETE_ARRAY(m_items);
}

template <class Type>
Cloud::Utils::DynamicArray<Type>& Cloud::Utils::DynamicArray<Type>::operator=(const DynamicArray<Type>& dynamicArray)
{
    Init(dynamicArray.m_maxCount, dynamicArray.m_growSize);

    for(CLint i = 0; i < dynamicArray.m_count; ++i)
    {
        m_items[i] = dynamicArray.m_items[i];
    }

    m_count = dynamicArray.m_count;
    m_last = &m_items[m_count];

    return *this;
}

template <class Type>
inline Type& Cloud::Utils::DynamicArray<Type>::operator[](const CLint& index)
{
    CL_ASSERT(index >= 0 && index < m_count, "Index is out of bounds!");
#pragma warning(suppress: 26481)
    return m_items[index];
}

template <class Type>
inline const Type& Cloud::Utils::DynamicArray<Type>::operator[](const CLint& index) const
{
    CL_ASSERT(index >= 0 && index < m_count, "Index is out of bounds!");
#pragma warning(suppress: 26481)
    return m_items[index];
}

template <class Type>
void Cloud::Utils::DynamicArray<Type>::Init(const CLint& startSize, const CLint& growSize)
{
    CL_ASSERT(growSize > 0, "Grow size can't be 0 or less!");

    SAFE_DELETE_ARRAY(m_items);

    m_maxCount = startSize;
    m_growSize = growSize;
    m_count = 0;

    m_items = new Type[m_maxCount];
    m_last = m_items;
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::Add(const Type& item)
{
    if (m_count >= m_maxCount)
    {
        Resize(m_count + m_growSize);
    }

    ++m_count;
#pragma warning(suppress: 26481)
    m_items[m_count - 1] = item;
#pragma warning(suppress: 26481)
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::Add(Type&& item)
{
    if (m_count >= m_maxCount)
    {
        Resize(m_count + m_growSize);
    }

    ++m_count;
#pragma warning(suppress: 26481)
    m_items[m_count - 1] = std::move(item);
#pragma warning(suppress: 26481)
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::Insert(const CLint& index, const Type& item)
{
    CL_ASSERT(index < m_count && index >= 0, "");

    if (m_count >= m_maxCount)
    {
        Resize(m_count + m_growSize);
    }

    for(CLint i = m_count; i >= index; --i)
    {
        m_items[i] = m_items[i - 1];
    }

    ++m_count;
    m_items[index] = item;
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::Remove(const Type& item)
{
    for (CLint i = 0; i < m_count; ++i)
    {
        if (m_items[i] == item)
        {
            RemoveAtIndex(i);
        }
    }
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveAtIndex(const CLint& index)
{
    CL_ASSERT(index >= 0 && index < m_count, "Removing out of bounds!");

    for (CLint i = index; i < m_count; ++i)
    {
        m_items[i] = m_items[i + 1];
    }

    --m_count;
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveCyclic(const Type& item)
{
    for (CLint i = 0; i < m_count; ++i)
    {
        if (m_items[i] == item)
        {
            RemoveCyclicAtIndex(i);
        }
    }
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveCyclicAtIndex(const CLint& index)
{
    CL_ASSERT(index > 0 && index <= m_count, "Removing out of bounds!");

    m_items[index] = m_items[m_count - 1];

    --m_count;
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveAll()
{
    m_count = 0;
    m_last = m_items;
}

template <class Type>
void Cloud::Utils::DynamicArray<Type>::Resize(CLint newSize)
{
    CL_ASSERT(newSize > 0, "New size can't be 0 or less!");
    CL_ASSERT(m_items != 0, "Trying to resize an uninitialised DynamicArray!");

#pragma warning(suppress: 26481)
    Type* oldList = &m_items[0];

    m_items = new Type [newSize];

    for(CLint i = 0; i < m_count; ++i)
    {
        m_items[i] = std::move(oldList[i]);
    }

    SAFE_DELETE_ARRAY(oldList);

    m_maxCount = newSize;
    m_last = &m_items[m_count];
}

template <class Type>
__forceinline CLint Cloud::Utils::DynamicArray<Type>::Count() const
{
    return m_count;
}

template <class Type>
inline Type* Cloud::Utils::DynamicArray<Type>::Last() const
{
    if (m_count > 0)
    {
        return m_last - 1;
    }

    return 0;
}

#endif // CLOUD_DATASTRUCTURES_DYNAMICARRAY_INLINE
