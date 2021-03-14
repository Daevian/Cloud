#ifndef CLOUD_MATH_FLOAT2_INLINE
#define CLOUD_MATH_FLOAT2_INLINE

#include <math.h>

inline Cloud::Math::Float2::Float2()
    : a{0.0f, 0.0f}
{
}

inline Cloud::Math::Float2::Float2(float xValue, float yValue)
    : a{xValue, yValue}
{
}

inline Cloud::Math::Float2::Float2(const Float2& float2)
    : a{float2.x, float2.y}
{
}

inline void Cloud::Math::Float2::Set(float xValue, float yValue)
{
    x = xValue;
    y = yValue;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator+(const Cloud::Math::Float2& float2) const
{
    return Float2(*this) += float2;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator-(const Cloud::Math::Float2& float2) const
{
    return Float2(*this) -= float2;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator*(const Cloud::Math::Float2& float2) const
{
    return Float2(*this) *= float2;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator/(const Cloud::Math::Float2& float2) const
{
    return Float2(*this) /= float2;
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator+=(const Cloud::Math::Float2& float2)
{
    this->x += float2.x;
    this->y += float2.y;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator-=(const Cloud::Math::Float2& float2)
{
    this->x -= float2.x;
    this->y -= float2.y;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator*=(const Cloud::Math::Float2& float2)
{
    this->x *= float2.x;
    this->y *= float2.y;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator/=(const Cloud::Math::Float2& float2)
{
    this->x /= float2.x;
    this->y /= float2.y;
    return(*this);
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator+(const float& scalar) const
{
    return Float2(*this) += scalar;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator-(const float& scalar) const
{
    return Float2(*this) -= scalar;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator*(const float& scalar) const
{
    return Float2(*this) *= scalar;
}

inline Cloud::Math::Float2 Cloud::Math::Float2::operator/(const float& scalar) const
{
    return Float2(*this) /= scalar;
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator+=(const float& scalar)
{
    this->x += scalar;
    this->y += scalar;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator-=(const float& scalar)
{
    this->x -= scalar;
    this->y -= scalar;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator*=(const float& scalar)
{
    this->x *= scalar;
    this->y *= scalar;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator/=(const float& scalar)
{
    this->x /= scalar;
    this->y /= scalar;
    return(*this);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::operator=(const Cloud::Math::Float2& float2)
{
    this->x = float2.x;
    this->y = float2.y;
    return(*this);
}

inline bool Cloud::Math::Float2::operator==(const Cloud::Math::Float2& float2) const
{
    if (this->x != float2.x) return false;
    if (this->y != float2.y) return false;
    return true;
}

inline float Cloud::Math::Float2::Dot(const Cloud::Math::Float2& float2) const
{
    float result = 0.0f;
    result += this->x * float2.x;
    result += this->y * float2.y;
    return result;
}

inline float Cloud::Math::Float2::Length() const
{
    return sqrtf(Length2());
}

inline float Cloud::Math::Float2::Length2() const
{
    return (x * x) + (y * y);
}

inline Cloud::Math::Float2& Cloud::Math::Float2::Normalize()
{
    float length = Length();
    if(length != 0.0f)
    {
        float inverseLength = 1.0f / length;
        x *= inverseLength;
        y *= inverseLength;
    }

    return *this;
}

#endif // CLOUD_MATH_FLOAT2_INLINE