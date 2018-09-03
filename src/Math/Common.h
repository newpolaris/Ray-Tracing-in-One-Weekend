#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <cassert>

namespace Math
{
	static const float Pi = glm::pi<float>();
	static const float PiOver2 = Pi/2;
	static const float PiOver4 = Pi/4;
	static const float InvPi = 1/Pi;
	static const float Inv2Pi = 1/(2*Pi);
	static const float Inv4Pi = 1/(4*Pi);
	static const float Sqrt2 = glm::sqrt(2);

    template <typename T> inline T AlignUpWithMask( T value, size_t mask )
    {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> inline T AlignDownWithMask( T value, size_t mask )
    {
        return (T)((size_t)value & ~mask);
    }

    template <typename T> inline T AlignUp( T value, size_t alignment )
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T> inline T AlignDown( T value, size_t alignment )
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    template <typename T> inline bool IsAligned( T value, size_t alignment )
    {
        return 0 == ((size_t)value & (alignment - 1));
    }

    template <typename T> inline T DivideByMultiple( T value, size_t alignment )
    {
        return (T)((value + alignment - 1) / alignment);
    }

    template <typename T> inline bool IsPowerOfTwo(T value)
    {
        return 0 == (value & (value - 1));
    }

    template <typename T> inline bool IsDivisible(T value, T divisor)
    {
        return (value / divisor) * divisor == value;
    }

	inline float Log2(float value)
	{
		return glm::log2(value);
	}

	inline uint8_t Log2(uint32_t value)
	{
	#if _WIN32
        unsigned long lz;
        _BitScanReverse(&lz, value);
		return lz;
	#else
		return 31 - __builtin_clz(value);
	#endif
	}

	inline uint8_t Log2(int32_t value)
	{
		assert(value >= 0);
		return Log2(uint32_t(value));
	}

    inline uint8_t Log2(uint64_t value)
    {
	#if _WIN32
        unsigned long lz;
        _BitScanReverse64(&lz, value);
		return lz;
	#else
		return 63 - __builtin_clzll(value);
	#endif
    }

	inline uint8_t Log2(int64_t value)
	{
		assert(value >= 0);
		return Log2(uint64_t(value));
	}

    template <typename T> inline T AlignPowerOfTwo(T value)
    {
        return value == 0 ? 0 : 1 << Log2(value);
    }
}
