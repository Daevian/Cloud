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
            inline Float2(CLfloat xValue, CLfloat yValue);
            inline Float2(const Float2& float2);

            inline void Set(CLfloat xValue, CLfloat yValue);

            inline Float2 operator+(const Float2& float2) const;
            inline Float2 operator-(const Float2& float2) const;
            inline Float2 operator*(const Float2& float2) const;
            inline Float2 operator/(const Float2& float2) const;
            inline Float2& operator+=(const Float2& float2);
            inline Float2& operator-=(const Float2& float2);
            inline Float2& operator*=(const Float2& float2);
            inline Float2& operator/=(const Float2& float2);

            inline Float2 operator+(const CLfloat& scalar) const;
            inline Float2 operator-(const CLfloat& scalar) const;
            inline Float2 operator*(const CLfloat& scalar) const;
            inline Float2 operator/(const CLfloat& scalar) const;
            inline Float2& operator+=(const CLfloat& scalar);
            inline Float2& operator-=(const CLfloat& scalar);
            inline Float2& operator*=(const CLfloat& scalar);
            inline Float2& operator/=(const CLfloat& scalar);

            inline Float2& operator=(const Float2& float2);
            inline CLbool operator==(const Float2& float2) const;

            inline CLfloat Dot(const Float2& float2) const;

            inline CLfloat Length() const;
            inline CLfloat Length2() const;

            inline Float2& Normalize();

            union
            {
                std::array<CLfloat, 2> a;
                struct
                {
                    CLfloat x, y;
                };
            };
        };
    }
}

typedef Cloud::Math::Float2 ClFloat2;

#include "Float2.inl"

#endif // CLOUD_MATH_FLOAT2_HEADER