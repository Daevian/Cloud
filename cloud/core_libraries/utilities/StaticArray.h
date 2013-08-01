#ifndef CLOUD_DATASTRUCTURES_STATICARRAY_HEADER
#define CLOUD_DATASTRUCTURES_STATICARRAY_HEADER

#include "DefinesTypes.h"

namespace Cloud
{
    namespace Utils
    {
        template<class Type, CLint Size>
        class StaticArray
        {
        public:
            StaticArray();
            StaticArray(const StaticArray& staticArray);
            ~StaticArray();

            StaticArray<Type, Size>& operator=(const StaticArray<Type, Size>& staticArray);

            inline Type& operator[](const CLint& index);
            inline const Type& operator[](const CLint& index) const;

            inline void Insert(const CLint& index, const Type& item);

            CLint Count()   { return Size; }
            CLuint SizeOf() { return sizeof(Type) * Size; }

        protected:
            Type m_items[Size];

        };
    }
}

#include "StaticArray.inl"

#endif // CLOUD_DATASTRUCTURES_STATICARRAY_HEADER