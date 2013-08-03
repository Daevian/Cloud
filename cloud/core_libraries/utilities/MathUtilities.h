#ifndef CLOUD_MATH_MATH_UTILITIES_HEADER
#define CLOUD_MATH_MATH_UTILITIES_HEADER

#include <DirectXMath.h>
#include "DefinesTypes.h"

namespace Cloud
{
    namespace Math
    {
        inline CLfloat ToRadians(const CLfloat degrees)
        {
            return DirectX::XMConvertToRadians(degrees);
        }

        inline CLfloat ToDegrees(const CLfloat radians)
        {
            return DirectX::XMConvertToDegrees(radians);
        }
    }
}

#endif // CLOUD_MATH_MATH_UTILITIES_HEADER