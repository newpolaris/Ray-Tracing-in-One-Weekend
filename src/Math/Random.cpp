#include "Random.h"
#include <cmath>
#include <Math/RNG.h>
#include <random>
#include <thread>

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923
#endif

namespace {
	float Rand()
	{
#ifdef _WIN32
		return float(rand()) / (RAND_MAX + 1.f);
#else
		return drand48();
#endif
	}

	float RandPbrt()
	{
		thread_local size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
		thread_local Math::RNG rng(id);
		return rng.UniformFloat();
	}

	struct Twister
	{
		Twister() noexcept 
			: m_Engine(std::random_device{}())
			, m_Dist(0, 1)
		{
			size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
			m_Engine.seed(id);
		}
		
		Float operator()() noexcept
		{
			return m_Dist(m_Engine);
		}

		std::mt19937 m_Engine;
		std::uniform_real_distribution<Float> m_Dist;
	};

	float RandMt19937()
	{
		thread_local Twister twister;
		return twister();
	}
}

float Math::BaseRandom()
{
    return RandPbrt();
    return RandMt19937();
	return Rand();
}

glm::vec2 Math::randomUnitDisk()
{
	glm::vec2 p;
	do {
		p = 2.f * glm::vec2(BaseRandom(), BaseRandom()) - 1.f;
	} while (glm::dot(p, p) >= 1.f);
	return p;
}

// [OneWeek]
glm::vec3 Math::randomUnitSphere()
{
	glm::vec3 p;
	do {
		p = 2.f * glm::vec3(BaseRandom(), BaseRandom(), BaseRandom()) - 1.f;
	} while (glm::dot(p, p) >= 1.f);
	return p;
}

// [OneWeek]
glm::vec3 Math::randomCosineDirection()
{
	float r1 = BaseRandom();
	float r2 = BaseRandom();
	float x = cos(2 * M_PI * r1) * sqrt(r2);
	float y = sin(2 * M_PI * r1) * sqrt(r2);
	float z = sqrt(1 - r2);
	return glm::vec3(x, y, z);
}

// [OneWeek]
float Math::randomCosineDirectionPdf(float cosTheta)
{
	return cosTheta / M_PI;
}
