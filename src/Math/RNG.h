#pragma once

#include <cstdint>
#include <algorithm>

namespace Math {

class RNG
{
public:

	RNG() noexcept;
	RNG(uint64_t sequenceIndex) noexcept;

	void SetSequence(uint64_t sequenceIndex) noexcept;
	uint32_t UniformUInt32() noexcept;
	uint32_t UniformUInt32(uint32_t b) noexcept;
	float UniformFloat() noexcept;

private:

	uint64_t m_State;
	uint64_t m_Inc;
};

inline uint32_t RNG::UniformUInt32(uint32_t b) noexcept
{
	uint32_t threshold = (~b + 1u) % b;
	while (true)
	{
		uint32_t r = UniformUInt32();
		if (r >= threshold)
			return r % b;
	}
}

}
