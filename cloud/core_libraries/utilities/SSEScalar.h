#ifndef CLOUD_MATH_SCALAR_HEADER
#define CLOUD_MATH_SCALAR_HEADER

#include "DefinesTypes.h"
#include "DefinesMacros.h"
#include <xmmintrin.h>

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
                inline Scalar(__m128 vector);
                explicit inline Scalar(CLfloat scalar);

                inline operator CLfloat() const;


                CL_ALIGN(16) __m128 m_scalar;
            };

            inline Scalar::Scalar(__m128 vector)
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