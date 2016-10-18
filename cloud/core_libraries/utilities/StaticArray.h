#ifndef CLOUD_DATASTRUCTURES_STATICARRAY_HEADER
#define CLOUD_DATASTRUCTURES_STATICARRAY_HEADER

#include "DefinesTypes.h"
#include <array>

namespace Cloud
{
    namespace Utils
    {
        template<class Type, CLuint Size>
        class StaticArray
        {
        public:
            StaticArray();
            StaticArray(const StaticArray& staticArray);
            ~StaticArray();

            StaticArray<Type, Size>& operator=(const StaticArray<Type, Size>& staticArray);

            inline Type& operator[](const CLuint& index);
            inline const Type& operator[](const CLuint& index) const;

            inline void Insert(const CLuint& index, const Type& item);

            CLuint Count()   { return Size; }
            CLuint SizeOf() { return sizeof(Type) * Size; }
            Type*  GetBuffer() { return &m_items[0]; }

        protected:
            std::array<Type, Size> m_items;

        };
    }
}

#include "StaticArray.inl"

#endif // CLOUD_DATASTRUCTURES_STATICARRAY_HEADER