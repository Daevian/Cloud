#ifndef CLOUD_MATH_VECTOR_MATH_INLINE
#define CLOUD_MATH_VECTOR_MATH_INLINE

inline Cloud::Math::Vector4 Cloud::Math::VectorSet(CLfloat value0, CLfloat value1, CLfloat value2, CLfloat value3)
{
    return _mm_set_ps(value0, value1, value2, value3);
}

inline Cloud::Math::Vector4 Cloud::Math::VectorAdd(const Vector4& vec1, const Vector4& vec2)
{
    return _mm_add_ps(vec1, vec2);
}

inline Cloud::Math::Vector4 Cloud::Math::VectorMul(const Vector4& vec1, const Vector4& vec2)
{
    return _mm_mul_ps(vec1, vec2);
}

inline Cloud::Math::Vector4 Cloud::Math::VectorMin(const Vector4& vec1, const Vector4& vec2)
{
    return _mm_min_ps(vec1, vec2);
}

inline Cloud::Math::Vector4 Cloud::Math::VectorMax(const Vector4& vec1, const Vector4& vec2)
{
    return _mm_max_ps(vec1, vec2);
}

#endif // CLOUD_MATH_VECTOR_MATH_INLINE