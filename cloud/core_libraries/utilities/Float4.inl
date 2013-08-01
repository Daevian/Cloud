#ifndef CLOUD_MATH_FLOAT4_INLINE
#define CLOUD_MATH_FLOAT4_INLINE

#include <math.h>

inline Cloud::Math::Float4::Float4(CLfloat value)
:x(value)
,y(value)
,z(value)
,w(value)
{
}

inline Cloud::Math::Float4::Float4(CLfloat xValue, CLfloat yValue, CLfloat zValue, CLfloat wValue)
:x(xValue)
,y(yValue)
,z(zValue)
,w(wValue)
{
}

inline Cloud::Math::Float4::Float4(const Float4& float4)
{
    v = float4.v;
}

inline void Cloud::Math::Float4::Set(CLfloat xValue, CLfloat yValue, CLfloat zValue, CLfloat wValue)
{
    x = xValue;
    y = yValue;
    z = zValue;
    w = wValue;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator+(const Float4& float4) const
{
    return Float4(*this) += float4;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator-(const Float4& float4) const
{
    return Float4(*this) -= float4;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator+=(const Float4& float4)
{
    this->v = _mm_add_ps(this->v, float4.v);
    return *this;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator-=(const Float4& float4)
{
    this->v = _mm_sub_ps(this->v, float4.v);
    return *this;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator+(const CLfloat& scalar) const
{
    return Float4(*this) += scalar;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator-(const CLfloat& scalar) const
{
    return Float4(*this) -= scalar;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator*(const CLfloat& scalar) const
{
    return Float4(*this) *= scalar;
}

inline Cloud::Math::Float4 Cloud::Math::Float4::operator/(const CLfloat& scalar) const
{
    return Float4(*this) /= scalar;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator+=(const CLfloat& scalar)
{
    SSE::Scalar sseScalar(scalar);
    this->v = _mm_add_ps(this->v, sseScalar.m_scalar);
    return *this;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator-=(const CLfloat& scalar)
{
    SSE::Scalar sseScalar(scalar);
    this->v = _mm_sub_ps(this->v, sseScalar.m_scalar);
    return *this;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator*=(const CLfloat& scalar)
{
    SSE::Scalar sseScalar(scalar);
    this->v = _mm_mul_ps(this->v, sseScalar.m_scalar);
    return *this;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator/=(const CLfloat& scalar)
{
    SSE::Scalar sseScalar(scalar);
    this->v = _mm_div_ps(this->v, sseScalar.m_scalar);
    return *this;
}

inline Cloud::Math::Float4& Cloud::Math::Float4::operator=(const Float4& float4)
{
    this->x = float4.x;
    this->y = float4.y;
    this->z = float4.z;
    this->w = float4.w;
    return *this;
}

inline CLbool Cloud::Math::Float4::operator==(const Float4& float4) const
{
    if (this->x != float4.x) return false;
    if (this->y != float4.y) return false;
    if (this->z != float4.z) return false;
    if (this->w != float4.w) return false;
    return true;
}

Cloud::Math::Float4 Cloud::Math::Float4::Cross(const Float4& float4) const
{
    Float4 result;
    result.x = (this->y * float4.z) - (this->z * float4.y);
    result.y = -((this->x * float4.z) - (this->z * float4.x));
    result.z = (this->x * float4.y) - (this->y * float4.x);
    result.w = 0.0f;
    return result;
}

CLfloat Cloud::Math::Float4::Dot(const Float4& float4) const
{
    CLfloat result = 0.0f;
    result += this->x * float4.x;
    result += this->y * float4.y;
    result += this->z * float4.z;
    result += this->w * float4.w;
    return result;
}

CLfloat Cloud::Math::Float4::Length() const
{
    return sqrtf(Length2());
}

CLfloat Cloud::Math::Float4::Length2() const
{
    return (x * x) + (y * y) + (z * z) + (w * w);
}

Cloud::Math::Float4& Cloud::Math::Float4::Normalize()
{
    CLfloat length = Length();
    if(length != 0.0f)
    {
        CLfloat inverseLength = 1.0f / length;
        *this *= inverseLength;
    }

    return *this;
}

#endif // CLOUD_MATH_FLOAT4_INLINE