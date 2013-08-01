#ifndef CLOUD_MATH_MATRIX4_HEADER
#define CLOUD_MATH_MATRIX4_HEADER

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

            inline static Matrix4 Identity();
            inline static Matrix4 Orthographic(CLfloat width, CLfloat height, CLfloat nearClip, CLfloat farClip);

            Float4 m_col0;
            Float4 m_col1;
            Float4 m_col2;
            Float4 m_col3;
        };
    }
}

#include "Matrix4.inl"

#endif // CLOUD_MATH_MATRIX4_HEADER