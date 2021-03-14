#ifndef CLOUD_MATH_FLOAT4_HEADER
#define CLOUD_MATH_FLOAT4_HEADER

#include <DirectXMath.h>
#include "SSEScalar.h"
#include "Float2.h"
#include "Float3.h"

#pragma warning (disable:4201)

namespace Cloud
{
    namespace Math
    {
        CL_ALIGN(16)
        class Float4
        {
        public:
            Float4() { };
            inline Float4(float value);
            inline Float4(float xValue, float yValue, float zValue, float wValue);
            inline Float4(const Float4& float4);
            inline Float4(const Float3& float3, float valW);
            inline Float4(const Float2& float2, float valZ, float valW);
            inline Float4(const Vector4& vector4);

            inline void Set(float xValue, float yValue, float zValue, float wValue);

            inline Float4 operator+(const Float4& float4) const;
            inline Float4 operator-(const Float4& float4) const;
            inline Float4 operator*(const Float4& float4) const;
            inline Float4& operator+=(const Float4& float4);
            inline Float4& operator-=(const Float4& float4);
            inline Float4& operator*=(const Float4& float4);

            inline Float4 operator+(const float& scalar) const;
            inline Float4 operator-(const float& scalar) const;
            inline Float4 operator*(const float& scalar) const;
            inline Float4 operator/(const float& scalar) const;
            inline Float4& operator+=(const float& scalar);
            inline Float4& operator-=(const float& scalar);
            inline Float4& operator*=(const float& scalar);
            inline Float4& operator/=(const float& scalar);

            inline Float4& operator=(const Float4& float4);
            inline bool operator==(const Float4& float4) const;

            inline Float4 Cross(const Float4& float4) const;
            inline float Dot(const Float4& float4) const;

            inline float Length() const;
            inline float Length2() const;

            inline Float4& Normalize();

            inline const DirectX::XMVECTOR& GetDxVector() const;
            inline Float3 GetXYZ() const;

            CL_ALIGN(16) union
            {
                DirectX::XMVECTOR m_dxVector;
                Vector4 v;
                float a[4];
                struct
                {
                    float x, y, z, w;
                };
            };
        };
    }
}

typedef Cloud::Math::Float4 float4;

#include "Float4.inl"

#endif // CLOUD_MATH_FLOAT4_HEADER