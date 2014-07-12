#ifndef CLOUD_MATH_SCALAR_HEADER
#define CLOUD_MATH_SCALAR_HEADER

#include "VectorMath.h"

namespace Cloud
{
    namespace Math
    {
        namespace SSE
        {
            CL_ALIGN(16)
            class Scalar
            {
            public:
                inline Scalar() { }
                inline Scalar(Vector4 vector);
                explicit inline Scalar(CLfloat scalar);

                inline operator CLfloat() const;


                CL_ALIGN(16) Vector4 m_scalar;
            };

            inline Scalar::Scalar(Vector4 vector)
                :m_scalar(vector)
            {
            }

            inline Scalar::Scalar(CLfloat scalar)
            {
                m_scalar = _mm_set1_ps(scalar);
            }

            inline Scalar::operator CLfloat() const
            {
                return *((float*)&m_scalar);
            }
        }
    }
}

#endif // CLOUD_MATH_SCALAR_HEADER