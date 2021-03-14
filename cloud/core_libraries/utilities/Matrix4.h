#ifndef CLOUD_MATH_MATRIX4_HEADER
#define CLOUD_MATH_MATRIX4_HEADER

#include <DirectXMath.h>
#include "Float4.h"

namespace Cloud
{
    namespace Math
    {
        CL_ALIGN(16)
        class Matrix4
        {
        public:
            Matrix4() { };
            inline Matrix4(const Float4& col0, const Float4& col1, const Float4& col2, const Float4& col3);

            inline Matrix4 operator*(const Matrix4& matrix) const;
            inline Float4 operator*(const Float4& float4) const;
            inline Matrix4& operator*=(const Matrix4& matrix);

            inline Float4 GetTranslation() const;

            inline const Vector4& GetCol0() const;
            inline const Vector4& GetCol1() const;
            inline const Vector4& GetCol2() const;
            inline const Vector4& GetCol3() const;

            inline void SetCol0(const Float4& col);
            inline void SetCol1(const Float4& col);
            inline void SetCol2(const Float4& col);
            inline void SetCol3(const Float4& col);

            inline void SetUpper3x3(const Matrix4& matrix);

            inline static Matrix4 Identity();
            inline static Matrix4 Multiply(const Matrix4& matrix1, const Matrix4& matrix2);
            inline static Matrix4 Orthographic(float width, float height, float nearClip, float farClip);
            inline static Matrix4 Perspective(float fovY, float aspectRatio, float nearClip, float farClip);
            inline static Matrix4 Inverse(const Matrix4& matrix);
            inline static Matrix4 Translation(float x, float y, float z);
            inline static Matrix4 Translation(const Float4& translation);
            inline static Matrix4 Transpose(const Matrix4& matrix);
            inline static Matrix4 Rotation(float x, float y, float z);
            inline static Matrix4 Rotation(const Float4& rotation);
            inline static Matrix4 Scale(float x, float y, float z);
            inline static Matrix4 Scale(const Float4& rotation);
            inline static Matrix4 LookTo(const Float4& eyePosition, const Float4& direction, const Float4& up);

        private:
            inline Matrix4(const DirectX::XMMATRIX& dxMatrix);
            inline Matrix4& operator=(const DirectX::XMMATRIX& dxMatrix);

            DirectX::XMMATRIX m_dxMatrix;
        };
    }
}

typedef Cloud::Math::Matrix4 ClMatrix4;

#include "Matrix4.inl"

#endif // CLOUD_MATH_MATRIX4_HEADER