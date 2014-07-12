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
            inline Float4(CLfloat value);
            inline Float4(CLfloat xValue, CLfloat yValue, CLfloat zValue, CLfloat wValue);
            inline Float4(const Float4& float4);
            inline Float4(const Float3& float3, CLfloat valW);
            inline Float4(const Float2& float2, CLfloat valZ, CLfloat valW);
            inline Float4(const Vector4& vector4);

            inline void Set(CLfloat xValue, CLfloat yValue, CLfloat zValue, CLfloat wValue);

            inline Float4 operator+(const Float4& float4) const;
            inline Float4 operator-(const Float4& float4) const;
            inline Float4 operator*(const Float4& float4) const;
            inline Float4& operator+=(const Float4& float4);
            inline Float4& operator-=(const Float4& float4);
            inline Float4& operator*=(const Float4& float4);

            inline Float4 operator+(const CLfloat& scalar) const;
            inline Float4 operator-(const CLfloat& scalar) const;
            inline Float4 operator*(const CLfloat& scalar) const;
            inline Float4 operator/(const CLfloat& scalar) const;
            inline Float4& operator+=(const CLfloat& scalar);
            inline Float4& operator-=(const CLfloat& scalar);
            inline Float4& operator*=(const CLfloat& scalar);
            inline Float4& operator/=(const CLfloat& scalar);

            inline Float4& operator=(const Float4& float4);
            inline CLbool operator==(const Float4& float4) const;

            inline Float4 Cross(const Float4& float4) const;
            inline CLfloat Dot(const Float4& float4) const;

            inline CLfloat Length() const;
            inline CLfloat Length2() const;

            inline Float4& Normalize();

            inline const DirectX::XMVECTOR& GetDxVector() const;
            inline Float3 GetXYZ() const;

            CL_ALIGN(16) union
            {
                DirectX::XMVECTOR m_dxVector;
                Vector4 v;
                CLfloat a[4];
                struct
                {
                    CLfloat x, y, z, w;
                };
            };
        };
    }
}

typedef Cloud::Math::Float4 ClFloat4;

#include "Float4.inl"

#endif // CLOUD_MATH_FLOAT4_HEADER