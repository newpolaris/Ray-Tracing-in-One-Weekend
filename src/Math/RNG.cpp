/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.
    This file is part of pbrt.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "RNG.h"

#ifndef _WIN32
#define PBRT_HAVE_HEX_FP_CONSTANTS
#endif

using namespace Math;

// Random Number Declarations
#ifndef PBRT_HAVE_HEX_FP_CONSTANTS
static const float FloatOneMinusEpsilon = 0.99999994;
#else
static const float FloatOneMinusEpsilon = 0x1.fffffep-1;
#endif

static const float OneMinusEpsilon = FloatOneMinusEpsilon;

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

float RNG::UniformFloat() noexcept
{
#ifndef PBRT_HAVE_HEX_FP_CONSTANTS
    float f = float(UniformUInt32() * 2.3283064365386963e-10f);
#else
    float f = float(OneMinusEpsilon, Float(UniformUInt32() * 0x1p-32f);
#endif
	return f < OneMinusEpsilon ? f : OneMinusEpsilon;
}
