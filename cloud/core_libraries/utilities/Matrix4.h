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

            inline void SetCol0(const Float4& col);
            inline void SetCol1(const Float4& col);
            inline void SetCol2(const Float4& col);
            inline void SetCol3(const Float4& col);

            inline static Matrix4 Identity();
            inline static Matrix4 Multiply(const Matrix4& matrix1, const Matrix4& matrix2);
            inline static Matrix4 Orthographic(CLfloat width, CLfloat height, CLfloat nearClip, CLfloat farClip);
            inline static Matrix4 Perspective(CLfloat fovY, CLfloat aspectRatio, CLfloat nearClip, CLfloat farClip);
            inline static Matrix4 Inverse(const Matrix4& matrix);
            inline static Matrix4 Translation(CLfloat x, CLfloat y, CLfloat z);
            inline static Matrix4 Translation(const Float4& translation);
            inline static Matrix4 Transpose(const Matrix4& matrix);

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