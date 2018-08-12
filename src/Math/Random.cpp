#include "Random.h"
#include <cmath>
#include <Math/RNG.h>
#include <random>
#include <thread>

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

float Math::BaseRandom()
{
    return RandPbrt();
    return RandMt19937();
}

glm::vec2 Math::randomUnitDisk()
{
	glm::vec2 p;
	do {
		p = 2.f * glm::vec2(BaseRandom(), BaseRandom()) - 1.f;
	} while (glm::dot(p, p) >= 1.f);
	return p;
}

glm::vec3 Math::randomUnitSphere()
{
	glm::vec3 p;
	do {
		p = 2.f * glm::vec3(BaseRandom(), BaseRandom(), BaseRandom()) - 1.f;
	} while (glm::dot(p, p) >= 1.f);
	return p;
}

