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

	float triliearInterp(float c[2][2][2], const glm::vec3& h)
	{
		float accum = 0.f;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					accum += (i*h.x + (1-i)*(1-h.x))
						   * (j*h.y + (1-j)*(1-h.y))
						   * (k*h.z + (1-k)*(1-h.z))
						   * c[i][j][k];
				}
		return accum;
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
	auto H = glm::floor(position);
	h = h*h*(3.f - 2.f*h);

	float c[2][2][2];
	for (int di = 0; di < 2; di++)
		for (int dj = 0; dj < 2; dj++)
			for (int dk = 0; dk < 2; dk++)
			{
				auto idx = s_PermX[(int(H.x)+di) & 0xFF]
						^  s_PermY[(int(H.y)+dj) & 0xFF]
						^  s_PermZ[(int(H.z)+dk) & 0xFF];
				c[di][dj][dk] = s_Ranfloat[idx];
			}
	return triliearInterp(c, h);;
}

