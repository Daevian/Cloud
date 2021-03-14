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
#define CL_PI 3.14159265359f

namespace Cloud
{
namespace Math
{
    inline float ToRadians(const float degrees)
    {
        return DirectX::XMConvertToRadians(degrees);
    }

    inline float ToDegrees(const float radians)
    {
        return DirectX::XMConvertToDegrees(radians);
    }

    template <class TYPE>
    inline const TYPE& Min(const TYPE& a, const TYPE& b)
    {
        return std::min(a, b);
    };

    inline Float4 Min(const Float4& a, const Float4& b)
    {
        return Float4(VectorMin(a.v, b.v));
    };

    template <class TYPE>
    inline const TYPE& Max(const TYPE& a, const TYPE& b)
    {
        return std::max(a, b);
    };

    inline Float4 Max(const Float4& a, const Float4& b)
    {
        return Float4(VectorMax(a.v, b.v));
    };

    template <class TYPE>
    inline const TYPE Clamp(const TYPE& max, const TYPE& min, const TYPE& val)
    {
        return ClMin(ClMax(val, max), min);
    };

    template <class TYPE>
    inline TYPE Abs(const TYPE& a)
    {
        return abs(a);
    };

    inline bool IsPowerOfTwo(uint value)
    {
        return (value != 0) && ((value & (value - 1)) == 0);
    }

    template <uint BOUNDRARY>
    inline uint RoundNearest(uint value)
    {
        CL_ASSERT(IsPowerOfTwo(BOUNDRARY), "Boundrary has to be power of two!");
        return (value + BOUNDRARY - 1) & ~(BOUNDRARY - 1);
    }
}
}

#endif // CLOUD_MATH_MATH_UTILITIES_HEADER