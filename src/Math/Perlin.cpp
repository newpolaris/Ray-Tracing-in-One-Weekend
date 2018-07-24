#include "Perlin.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <Math/Random.h>

namespace perlin
{
	const size_t n = 256;
	const size_t n_1 = n - 1;

	std::vector<float> s_Ranfloat;
	std::vector<int> s_PermX, s_PermY, s_PermZ;

	std::vector<int> generatePerm()
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::vector<int> perm(perlin::n);
		std::iota(std::begin(perm), std::end(perm), 0);
		std::shuffle(std::begin(perm), std::end(perm), g);
		return perm;
	}
}

void perlin::initialize()
{
	s_Ranfloat = std::vector<float>(perlin::n);
	std::generate(std::begin(s_Ranfloat), std::end(s_Ranfloat), Math::BaseRandom);
	s_PermX = generatePerm();
	s_PermY = generatePerm();
	s_PermZ = generatePerm();
}
	
void perlin::shutdown()
{
}

using namespace perlin;

Perlin::Perlin()
{
	assert(s_Ranfloat.size() > 0);
	assert(s_PermX.size() > 0);
	assert(s_PermY.size() > 0);
	assert(s_PermZ.size() > 0);
}


float Perlin::noise(const glm::vec3& position) const
{
	auto h = position - glm::floor(position);
	int i = int(4*h.x) & perlin::n_1;
	int j = int(4*h.y) & perlin::n_1;
	int k = int(4*h.z) & perlin::n_1;

	return s_Ranfloat[s_PermX[i] ^ s_PermY[j] ^ s_PermZ[k]];
}

