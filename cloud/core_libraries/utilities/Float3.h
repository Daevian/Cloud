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
            Float3(CLfloat xValue, CLfloat yValue, CLfloat zValue);
            Float3(const Float3& float3);

            inline void Set(CLfloat xValue, CLfloat yValue, CLfloat zValue);

            inline Float3 operator+(const Float3& float3) const;
            inline Float3 operator-(const Float3& float3) const;
            inline Float3& operator+=(const Float3& float3);
            inline Float3& operator-=(const Float3& float3);

            inline Float3 operator+(const CLfloat& scalar) const;
            inline Float3 operator-(const CLfloat& scalar) const;
            inline Float3 operator*(const CLfloat& scalar) const;
            inline Float3 operator/(const CLfloat& scalar) const;
            inline Float3& operator+=(const CLfloat& scalar);
            inline Float3& operator-=(const CLfloat& scalar);
            inline Float3& operator*=(const CLfloat& scalar);
            inline Float3& operator/=(const CLfloat& scalar);

            inline Float3& operator=(const Float3& float3);

            inline CLbool operator==(const Float3& float3) const;

            inline Float3 Cross(const Float3& float3) const;
            inline CLfloat Dot(const Float3& float3) const;

            inline CLfloat Length() const;
            inline CLfloat Length2() const;

            inline Float3& Normalize();

            union
            {
                CLfloat a[3];
                struct
                {
                    CLfloat x, y, z;
                };
            };
        };
    }
}

#pragma warning (default : 4201)

typedef Cloud::Math::Float3 ClFloat3;

#include "Float3.inl"

#endif // CLOUD_MATH_FLOAT3_HEADER