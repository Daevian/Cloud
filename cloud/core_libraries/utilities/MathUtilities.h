#ifndef CLOUD_MATH_MATH_UTILITIES_HEADER
#define CLOUD_MATH_MATH_UTILITIES_HEADER

#include <DirectXMath.h>
#include "DefinesTypes.h"
#include "VectorMath.h"
#include "Float4.h"

#define ClMin(a, b) Cloud::Math::Min(a, b)
#define ClMax(a, b) Cloud::Math::Max(a, b)
#define ClClamp(min, max, val) Cloud::Math::Clamp(min, max, val)
#define ClAbs(a) Cloud::Math::Abs(a)

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

    template <class TYPE>
    inline const TYPE& Min(const TYPE& a, const TYPE& b)
    {
        return min(a, b);
    };

    inline Float4 Min(const Float4& a, const Float4& b)
    {
        return Float4(VectorMin(a.v, b.v));
    };

    template <class TYPE>
    inline const TYPE& Max(const TYPE& a, const TYPE& b)
    {
        return max(a, b);
    };

    inline Float4 Max(const Float4& a, const Float4& b)
    {
        return Float4(VectorMax(a.v, b.v));
    };

    template <class TYPE>
    inline const TYPE& Clamp(const TYPE& max, const TYPE& min, const TYPE& val)
    {
        return ClMin(ClMax(val, max), min);
    }; 

    template <class TYPE>
    inline TYPE Abs(const TYPE& a)
    {
        return abs(a);
    };
}
}

#endif // CLOUD_MATH_MATH_UTILITIES_HEADER