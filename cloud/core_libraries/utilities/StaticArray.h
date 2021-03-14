#ifndef CLOUD_DATASTRUCTURES_STATICARRAY_HEADER
#define CLOUD_DATASTRUCTURES_STATICARRAY_HEADER

#include "DefinesTypes.h"
#include <array>

namespace Cloud
{
    namespace Utils
    {
        template<class Type, uint Size>
        class StaticArray
        {
        public:
            StaticArray();
            StaticArray(const StaticArray& staticArray);
            ~StaticArray();

            StaticArray<Type, Size>& operator=(const StaticArray<Type, Size>& staticArray);

            inline Type& operator[](const uint& index);
            inline const Type& operator[](const uint& index) const;

            inline void Insert(const uint& index, const Type& item);

            uint Count()   { return Size; }
            uint SizeOf() { return sizeof(Type) * Size; }
            Type*  GetBuffer() { return &m_items[0]; }

        protected:
            std::array<Type, Size> m_items;

        };
    }
}

#include "StaticArray.inl"

#endif // CLOUD_DATASTRUCTURES_STATICARRAY_HEADER