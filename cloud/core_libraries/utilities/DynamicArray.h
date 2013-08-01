#ifndef CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER
#define CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER

#include "DefinesTypes.h"

namespace Cloud
{
    namespace Utils
    {       
        template <class Type>
        class DynamicArray
        {
        public:
            DynamicArray();
            DynamicArray(const CLint& startSize, const CLint& growSize);
            DynamicArray(const DynamicArray& dynamicArray);
            ~DynamicArray();

            DynamicArray<Type>& operator=(const DynamicArray<Type>& dynamicArray);
            inline Type&        operator[](const CLint& index);
            inline const Type&  operator[](const CLint& index) const;

            void Init(const CLint& startSize, const CLint& growSize);

            inline void Add(const Type& item);
            inline void Insert(const CLint& index, const Type& item);

            inline void Remove(const Type& item);
            inline void RemoveAtIndex(const CLint& index);
            inline void RemoveCyclic(const Type& item);
            inline void RemoveCyclicAtIndex(const CLint& index);

            inline void RemoveAll();

            __forceinline CLint Count() const;
            inline Type* Last() const;

        protected:
            void Resize(CLint newSize);

            Type* m_items;
            Type* m_last;
            CLint m_count;
            CLint m_maxCount;
            CLint m_growSize;
        };
    }
}

#include "DynamicArray.inl"

#endif // CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER