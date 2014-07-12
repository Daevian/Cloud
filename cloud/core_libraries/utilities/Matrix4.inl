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

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::operator*(const Cloud::Math::Matrix4& matrix) const
{
    return Matrix4(matrix) *= (*this);
}

inline Cloud::Math::Float4 Cloud::Math::Matrix4::operator*(const Cloud::Math::Float4& float4) const
{
    return Float4(DirectX::XMVector4Transform(float4.m_dxVector, m_dxMatrix));
}

inline Cloud::Math::Matrix4& Cloud::Math::Matrix4::operator*=(const Cloud::Math::Matrix4& matrix)
{
    m_dxMatrix = DirectX::XMMatrixMultiply(m_dxMatrix, matrix.m_dxMatrix);
    return *this;
}

inline Cloud::Math::Float4 Cloud::Math::Matrix4::GetTranslation() const
{
    return Float4(GetCol3());
}

inline const Cloud::Math::Vector4& Cloud::Math::Matrix4::GetCol0() const
{
    return m_dxMatrix.r[0];
}

inline const Cloud::Math::Vector4& Cloud::Math::Matrix4::GetCol1() const
{
    return m_dxMatrix.r[1];
}

inline const Cloud::Math::Vector4& Cloud::Math::Matrix4::GetCol2() const
{
    return m_dxMatrix.r[2];
}

inline const Cloud::Math::Vector4& Cloud::Math::Matrix4::GetCol3() const
{
    return m_dxMatrix.r[3];
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

inline void Cloud::Math::Matrix4::SetUpper3x3(const Matrix4& matrix)
{
    CLfloat* source = (CLfloat*)&matrix.m_dxMatrix;
    CLfloat* target = (CLfloat*)&m_dxMatrix;

    target[0] = source[0];
    target[1] = source[1];
    target[2] = source[2];
    target[4] = source[4];
    target[5] = source[5];
    target[6] = source[6];
    target[8] = source[8];
    target[9] = source[9];
    target[10] = source[10];
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

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Rotation(CLfloat x, CLfloat y, CLfloat z)
{                            
    return DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Rotation(const Float4& rotation)
{                            
    return DirectX::XMMatrixRotationRollPitchYawFromVector(rotation.GetDxVector());
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Scale(CLfloat x, CLfloat y, CLfloat z)
{                            
    return DirectX::XMMatrixScaling(x, y, z);
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::Scale(const Float4& rotation)
{                            
    return DirectX::XMMatrixScalingFromVector(rotation.GetDxVector());
}

inline Cloud::Math::Matrix4 Cloud::Math::Matrix4::LookTo(const Float4& eyePosition, const Float4& direction, const Float4& up)
{     
    return DirectX::XMMatrixLookToLH(eyePosition.m_dxVector, direction.m_dxVector, up.m_dxVector);
}

#endif // CLOUD_MATH_MATRIX4_INLINE