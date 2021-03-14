#ifndef CLOUD_MATH_VECTOR_MATH_HEADER
#define CLOUD_MATH_VECTOR_MATH_HEADER

#include "DefinesTypes.h"
#include "DefinesMacros.h"
#include <xmmintrin.h>

namespace Cloud
{
namespace Math
{
    typedef __m128 Vector4;

    inline Vector4 VectorSet(float value0, float value1, float value2, float value3);

    inline Vector4 VectorAdd(const Vector4& vec1, const Vector4& vec2);
    inline Vector4 VectorMul(const Vector4& vec1, const Vector4& vec2);

    inline Vector4 VectorMin(const Vector4& vec1, const Vector4& vec2);
    inline Vector4 VectorMax(const Vector4& vec1, const Vector4& vec2);
}
}

typedef Cloud::Math::Vector4 ClVector4;

#include "VectorMath.inl"

#endif // CLOUD_MATH_VECTOR_MATH_HEADER