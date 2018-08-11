#include "Random.h"
#include <cmath>
#include <Math/RNG.h>
#include <random>
#include <thread>

float Rand()
{
	return float(rand()) / (RAND_MAX + 1.f);
}

float RandPbrt()
{
	thread_local size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
	thread_local Math::RNG rng(id);
	return rng.UniformFloat();
}

float RandMt19937()
{
	thread_local size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
	thread_local std::mt19937 engine(std::random_device{}());
	engine.seed(id);
	std::uniform_real_distribution<Float> dist(0, 1);
	return dist(engine);
}

float Math::BaseRandom()
{
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

