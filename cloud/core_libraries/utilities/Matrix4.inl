#ifndef CLOUD_MATH_MATRIX4_INLINE
#define CLOUD_MATH_MATRIX4_INLINE

inline Cloud::Math::Matrix4::Matrix4(const Float4& col0, const Float4& col1, const Float4& col2, const Float4& col3)
    : m_col0(col0)
    , m_col1(col1)
    , m_col2(col2)
    , m_col3(col3)
{
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Identity()
{
    Matrix4 identityMatrix( Float4(1.0f, 0.0f, 0.0f, 0.0f),
                            Float4(0.0f, 1.0f, 0.0f, 0.0f),
                            Float4(0.0f, 0.0f, 1.0f, 0.0f),
                            Float4(0.0f, 0.0f, 0.0f, 1.0f));
    return identityMatrix;
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Orthographic(CLfloat w, CLfloat h, CLfloat n, CLfloat f)
{
    Matrix4 orthographicMatrix( Float4(1.0f / w,    0.0f,       0.0f,               0.0f),
                                Float4(0.0f,        1.0f / h,   0.0f,               0.0f),
                                Float4(0.0f,        0.0f,       (-2.0f) / (f - n),  -((f + n) / (f - n))),
                                Float4(0.0f,        0.0f,       0.0f,               1.0f));
                                
    return orthographicMatrix;
}

#endif // CLOUD_MATH_MATRIX4_INLINE