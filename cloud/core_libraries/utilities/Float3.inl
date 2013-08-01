#ifndef CLOUD_MATH_FLOAT3_INLINE
#define CLOUD_MATH_FLOAT3_INLINE

#include <math.h>

inline Cloud::Math::Float3::Float3()
:x(0)
,y(0)
,z(0)
{
}

inline Cloud::Math::Float3::Float3(CLfloat xValue, CLfloat yValue, CLfloat zValue)
:x(xValue)
,y(yValue)
,z(zValue)
{
}

inline Cloud::Math::Float3::Float3(const Float3& float3)
:x(float3.x)
,y(float3.y)
,z(float3.z)
{
}

inline void Cloud::Math::Float3::Set(CLfloat xValue, CLfloat yValue, CLfloat zValue)
{
    x = xValue;
    y = yValue;
    z = zValue;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator+(const Cloud::Math::Float3& float3) const
{
    return Float3(*this) += float3;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator-(const Cloud::Math::Float3& float3) const
{
    return Float3(*this) -= float3;
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator+=(const Cloud::Math::Float3& float3)
{
    this->x += float3.x;
    this->y += float3.y;
    this->z += float3.z;
    return(*this);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator-=(const Cloud::Math::Float3& float3)
{
    this->x -= float3.x;
    this->y -= float3.y;
    this->z -= float3.z;
    return(*this);
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator+(const CLfloat& scalar) const
{
    return Float3(*this) += scalar;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator-(const CLfloat& scalar) const
{
    return Float3(*this) -= scalar;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator*(const CLfloat& scalar) const
{
    return Float3(*this) *= scalar;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::operator/(const CLfloat& scalar) const
{
    return Float3(*this) /= scalar;
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator+=(const CLfloat& scalar)
{
    this->x += scalar;
    this->y += scalar;
    this->z += scalar;
    return(*this);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator-=(const CLfloat& scalar)
{
    this->x -= scalar;
    this->y -= scalar;
    this->z -= scalar;
    return(*this);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator*=(const CLfloat& scalar)
{
    this->x *= scalar;
    this->y *= scalar;
    this->z *= scalar;
    return(*this);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator/=(const CLfloat& scalar)
{
    this->x /= scalar;
    this->y /= scalar;
    this->z /= scalar;
    return(*this);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::operator=(const Cloud::Math::Float3& float3)
{
    this->x = float3.x;
    this->y = float3.y;
    this->z = float3.z;
    return(*this);
}

inline CLbool Cloud::Math::Float3::operator==(const Cloud::Math::Float3& float3) const
{
    if (this->x != float3.x) return false;
    if (this->y != float3.y) return false;
    if (this->z != float3.z) return false;
    return true;
}

inline Cloud::Math::Float3 Cloud::Math::Float3::Cross(const Cloud::Math::Float3& float3) const
{
    Float3 result;
    result.x = (this->y * float3.z) - (this->z * float3.y);
    result.y = -((this->x * float3.z) - (this->z * float3.x));
    result.z = (this->x * float3.y) - (this->y * float3.x);
    return result;
}

inline CLfloat Cloud::Math::Float3::Dot(const Cloud::Math::Float3& float3) const
{
    CLfloat result = 0.0f;
    result += this->x * float3.x;
    result += this->y * float3.y;
    result += this->z * float3.z;
    return result;
}

inline CLfloat Cloud::Math::Float3::Length() const
{
    return sqrtf(Length2());
}

inline CLfloat Cloud::Math::Float3::Length2() const
{
    return (x * x) + (y * y) + (z * z);
}

inline Cloud::Math::Float3& Cloud::Math::Float3::Normalize()
{
    CLfloat length = Length();
    if(length != 0.0f)
    {
        CLfloat inverseLength = 1.0f / length;
        x *= inverseLength;
        y *= inverseLength;
        z *= inverseLength;
    }

    return *this;
}

#endif // CLOUD_MATH_FLOAT3_INLINE