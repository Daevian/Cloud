#ifndef CLOUD_DATASTRUCTURES_DYNAMICARRAY_INLINE
#define CLOUD_DATASTRUCTURES_DYNAMICARRAY_INLINE

#include "DefinesMacros.h"
#include "DefinesAsserts.h"
#include "MathUtilities.h"

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray()
    : m_last(nullptr)
    , m_count(0)
    , m_maxCount(0)
    , m_growSize(1)
{
    Init(0, 1);
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray(const int& startSize, const int& growSize)
    : m_last(nullptr)
    , m_count(0)
    , m_maxCount(startSize)
    , m_growSize(growSize)
{
    Init(startSize, growSize);
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::DynamicArray(const DynamicArray& dynamicArray)
{
    *this = dynamicArray;
}

template <class Type>
Cloud::Utils::DynamicArray<Type>::~DynamicArray()
{
    m_last = 0;
}

template <class Type>
Cloud::Utils::DynamicArray<Type>& Cloud::Utils::DynamicArray<Type>::operator=(const DynamicArray<Type>& dynamicArray)
{
    Init(dynamicArray.m_maxCount, dynamicArray.m_growSize);

    for (int i = 0; i < dynamicArray.m_count; ++i)
    {
        m_items[i] = dynamicArray.m_items[i];
    }

    m_count = dynamicArray.m_count;
    m_last = &m_items[m_count];

    return *this;
}

template <class Type>
inline Type& Cloud::Utils::DynamicArray<Type>::operator[](const int& index)
{
    CL_ASSERT(index >= 0 && index < m_count, "Index is out of bounds!");
#pragma warning(suppress: 26481)
    return m_items[index];
}

template <class Type>
inline const Type& Cloud::Utils::DynamicArray<Type>::operator[](const int& index) const
{
    CL_ASSERT(index >= 0 && index < m_count, "Index is out of bounds!");
#pragma warning(suppress: 26481)
    return m_items[index];
}

template <class Type>
void Cloud::Utils::DynamicArray<Type>::Init(const int& startSize, const int& growSize)
{
    CL_ASSERT(growSize > 0, "Grow size can't be 0 or less!");

    m_maxCount = startSize;
    m_growSize = growSize;
    m_count = 0;

    m_items = std::make_unique<Type[]>(m_maxCount);
    m_last = m_items.get();
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
inline void Cloud::Utils::DynamicArray<Type>::Insert(const int& index, const Type& item)
{
    CL_ASSERT(index < m_count && index >= 0, "");

    if (m_count >= m_maxCount)
    {
        Resize(m_count + m_growSize);
    }

    for (int i = m_count; i >= index; --i)
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
    for (int i = 0; i < m_count; ++i)
    {
        if (m_items[i] == item)
        {
            RemoveAtIndex(i);
        }
    }
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveAtIndex(const int& index)
{
    CL_ASSERT(index >= 0 && index < m_count, "Removing out of bounds!");

    for (int i = index; i < m_count; ++i)
    {
        m_items[i] = m_items[i + 1];
    }

    --m_count;
    m_last = &m_items[m_count];
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveCyclic(const Type& item)
{
    for (int i = 0; i < m_count; ++i)
    {
        if (m_items[i] == item)
        {
            RemoveCyclicAtIndex(i);
        }
    }
}

template <class Type>
inline void Cloud::Utils::DynamicArray<Type>::RemoveCyclicAtIndex(const int& index)
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
    m_last = m_items.get();
}

template <class Type>
void Cloud::Utils::DynamicArray<Type>::Resize(int newSize)
{
    CL_ASSERT(newSize > 0, "New size can't be 0 or less!");
    CL_ASSERT(m_items.get() != 0, "Trying to resize an uninitialised DynamicArray!");

    auto oldList = std::move(m_items);

    m_items = std::make_unique<Type[]>(newSize);

    for (int i = 0, count = ClMax(m_count, newSize); i < count; ++i)
    {
        m_items[i] = std::move(oldList[i]);
    }

    m_maxCount = newSize;
    m_last = &m_items[m_count];
}

template <class Type>
__forceinline int Cloud::Utils::DynamicArray<Type>::Count() const
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
