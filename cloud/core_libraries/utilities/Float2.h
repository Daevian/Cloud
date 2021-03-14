#ifndef CLOUD_MATH_FLOAT2_HEADER
#define CLOUD_MATH_FLOAT2_HEADER

#include <array>

#pragma warning (disable:4201)

namespace Cloud
{
    namespace Math
    {
        class Float2
        {
        public:
            inline Float2();
            inline Float2(float xValue, float yValue);
            inline Float2(const Float2& float2);

            inline void Set(float xValue, float yValue);

            inline Float2 operator+(const Float2& float2) const;
            inline Float2 operator-(const Float2& float2) const;
            inline Float2 operator*(const Float2& float2) const;
            inline Float2 operator/(const Float2& float2) const;
            inline Float2& operator+=(const Float2& float2);
            inline Float2& operator-=(const Float2& float2);
            inline Float2& operator*=(const Float2& float2);
            inline Float2& operator/=(const Float2& float2);

            inline Float2 operator+(const float& scalar) const;
            inline Float2 operator-(const float& scalar) const;
            inline Float2 operator*(const float& scalar) const;
            inline Float2 operator/(const float& scalar) const;
            inline Float2& operator+=(const float& scalar);
            inline Float2& operator-=(const float& scalar);
            inline Float2& operator*=(const float& scalar);
            inline Float2& operator/=(const float& scalar);

            inline Float2& operator=(const Float2& float2);
            inline bool operator==(const Float2& float2) const;

            inline float Dot(const Float2& float2) const;

            inline float Length() const;
            inline float Length2() const;

            inline Float2& Normalize();

            union
            {
                std::array<float, 2> a;
                struct
                {
                    float x, y;
                };
            };
        };
    }
}

typedef Cloud::Math::Float2 float2;

#include "Float2.inl"

#endif // CLOUD_MATH_FLOAT2_HEADER