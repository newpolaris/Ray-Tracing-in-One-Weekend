#include "RNG.h"

// [PBRT]
namespace Math {

}

using namespace Math;

const Float OneMinusEpsilon = 0x1.fffffep-1;
// const Float OneMinusEpsilon = 0x1.fffffffffffffp-1;

#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL

RNG::RNG() noexcept 
	: m_State(PCG32_DEFAULT_STATE)
	, m_Inc(PCG32_DEFAULT_STREAM)
{
}

RNG::RNG(uint64_t initseq) noexcept
{
	m_State = 0u;
	m_Inc = (initseq << 1u) | 1u;
	UniformUInt32();
	m_State += PCG32_DEFAULT_STATE;
	UniformUInt32();
}

uint32_t RNG::UniformUInt32() noexcept
{
	uint64_t oldstate = m_State;
	m_State = oldstate * PCG32_MULT + m_Inc;
	uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
	uint32_t rot = (uint32_t)(oldstate >> 59u);
	return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}

Float RNG::UniformFloat() noexcept
{
	Float f = Float(UniformUInt32() * 0x1p-32f);
	return f < OneMinusEpsilon ? f : OneMinusEpsilon;
}
