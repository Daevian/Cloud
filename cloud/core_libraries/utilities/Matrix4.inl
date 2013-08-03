#ifndef CLOUD_MATH_MATRIX4_INLINE
#define CLOUD_MATH_MATRIX4_INLINE


inline Cloud::Math::Matrix4::Matrix4(const Float4& col0, const Float4& col1, const Float4& col2, const Float4& col3)
    : m_dxMatrix(col0.GetDxVector(), col1.GetDxVector(), col2.GetDxVector(), col3.GetDxVector())
{
}

inline Cloud::Math::Matrix4::Matrix4(const DirectX::XMMATRIX& dxMatrix)
    : m_dxMatrix(dxMatrix)
{
}

inline Cloud::Math::Matrix4& Cloud::Math::Matrix4::operator=(const DirectX::XMMATRIX& dxMatrix)
{
    m_dxMatrix = dxMatrix;
    return *this;
}

inline void Cloud::Math::Matrix4::SetCol0(const Float4& col)
{
    m_dxMatrix.r[0] = col.m_dxVector;
}

inline void Cloud::Math::Matrix4::SetCol1(const Float4& col)
{
    m_dxMatrix.r[1] = col.m_dxVector;
}

inline void Cloud::Math::Matrix4::SetCol2(const Float4& col)
{
    m_dxMatrix.r[2] = col.m_dxVector;
}

inline void Cloud::Math::Matrix4::SetCol3(const Float4& col)
{
    m_dxMatrix.r[3] = col.m_dxVector;
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Identity()
{
    return DirectX::XMMatrixIdentity();
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Multiply(const Matrix4& matrix1, const Matrix4& matrix2)
{
    return DirectX::XMMatrixMultiply(matrix1.m_dxMatrix, matrix2.m_dxMatrix);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Orthographic(CLfloat width, CLfloat height, CLfloat nearClip, CLfloat farClip)
{
    return DirectX::XMMatrixOrthographicLH(width, height, nearClip, farClip);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Perspective(CLfloat fovY, CLfloat aspectRatio, CLfloat nearClip, CLfloat farClip)
{
    return DirectX::XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearClip, farClip);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Inverse(const Matrix4& matrix)
{                            
    return DirectX::XMMatrixInverse(0, matrix.m_dxMatrix);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Translation(CLfloat x, CLfloat y, CLfloat z)
{
    return DirectX::XMMatrixTranslation(x, y, z);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Translation(const Float4& translation)
{
    return DirectX::XMMatrixTranslationFromVector(translation.m_dxVector);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Transpose(const Matrix4& matrix)
{                            
    return DirectX::XMMatrixTranspose(matrix.m_dxMatrix);
}

#endif // CLOUD_MATH_MATRIX4_INLINE