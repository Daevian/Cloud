#ifndef CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER
#define CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER

#include "DefinesTypes.h"
#include <vector>
#include <memory>

namespace Cloud
{
    namespace Utils
    {       
        template <class Type>
        class DynamicArray
        {
        public:
            DynamicArray();
            DynamicArray(const int& startSize, const int& growSize);
            DynamicArray(const DynamicArray& dynamicArray);
            ~DynamicArray();

            DynamicArray<Type>& operator=(const DynamicArray<Type>& dynamicArray);
            inline Type&        operator[](const int& index);
            inline const Type&  operator[](const int& index) const;

            void Init(const int& startSize, const int& growSize);

            inline void Add(const Type& item);
            inline void Add(Type&& item);
            inline void Insert(const int& index, const Type& item);

            inline void Remove(const Type& item);
            inline void RemoveAtIndex(const int& index);
            inline void RemoveCyclic(const Type& item);
            inline void RemoveCyclicAtIndex(const int& index);

            inline void RemoveAll();

            __forceinline int Count() const;
            inline Type* Last() const;

        protected:
            void Resize(int newSize);

            std::unique_ptr<Type[]> m_items;
            Type* m_last;
            int m_count;
            int m_maxCount;
            int m_growSize;
        };
    }

    template <class T_Type>
    using Vector = std::vector<T_Type>;
}

#include "DynamicArray.inl"

#endif // CLOUD_DATASTRUCTURES_DYNAMICARRAY_HEADER