#ifndef CLOUD_MATH_FLOAT3_HEADER
#define CLOUD_MATH_FLOAT3_HEADER

#include "DefinesTypes.h"
#pragma warning (disable : 4201)

namespace Cloud
{
    namespace Math
    {
        class Float3
        {
        public:
            Float3();
            Float3(float xValue, float yValue, float zValue);
            Float3(const Float3& float3);

            inline void Set(float xValue, float yValue, float zValue);

            inline Float3 operator+(const Float3& float3) const;
            inline Float3 operator-(const Float3& float3) const;
            inline Float3& operator+=(const Float3& float3);
            inline Float3& operator-=(const Float3& float3);

            inline Float3 operator+(const float& scalar) const;
            inline Float3 operator-(const float& scalar) const;
            inline Float3 operator*(const float& scalar) const;
            inline Float3 operator/(const float& scalar) const;
            inline Float3& operator+=(const float& scalar);
            inline Float3& operator-=(const float& scalar);
            inline Float3& operator*=(const float& scalar);
            inline Float3& operator/=(const float& scalar);

            inline Float3& operator=(const Float3& float3);

            inline bool operator==(const Float3& float3) const;

            inline Float3 Cross(const Float3& float3) const;
            inline float Dot(const Float3& float3) const;

            inline float Length() const;
            inline float Length2() const;

            inline Float3& Normalize();

            union
            {
                float a[3];
                struct
                {
                    float x, y, z;
                };
            };
        };
    }
}

#pragma warning (default : 4201)

typedef Cloud::Math::Float3 float3;

#include "Float3.inl"

#endif // CLOUD_MATH_FLOAT3_HEADER