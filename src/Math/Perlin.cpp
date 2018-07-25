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

	std::vector<glm::vec3> s_RanVec;
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

	float perlinInterp(glm::vec3 c[2][2][2], float u, float v, float w) 
	{
		float uu = u*u*(3-2*u);
		float vv = v*v*(3-2*v);
		float ww = w*w*(3-2*w);
		float accum = 0;
		for (int i=0; i < 2; i++)
			for (int j=0; j < 2; j++)
				for (int k=0; k < 2; k++) {
					glm::vec3 weight_v(u-i, v-j, w-k);
					accum += (i*uu + (1-i)*(1-uu))*
						(j*vv + (1-j)*(1-vv))*
						(k*ww + (1-k)*(1-ww))*dot(c[i][j][k], weight_v);
				}
		return accum;
	}
}

void perlin::initialize()
{
	s_RanVec = std::vector<glm::vec3>(perlin::n);
	std::generate(std::begin(s_RanVec), std::end(s_RanVec), 
			[](){
				auto v = glm::vec3(Math::BaseRandom(), Math::BaseRandom(), Math::BaseRandom());
				return glm::normalize(2.f*v - 1.f);
			});
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
	assert(s_RanVec.size() > 0);
	assert(s_PermX.size() > 0);
	assert(s_PermY.size() > 0);
	assert(s_PermZ.size() > 0);
}

float Perlin::noise(const glm::vec3& p) const
{
	using std::floor;

	float u = p.x - floor(p.x);
	float v = p.y - floor(p.y);
	float w = p.z - floor(p.z);
	int i = floor(p.x);
	int j = floor(p.y);
	int k = floor(p.z);
	glm::vec3 c[2][2][2];
	for (int di=0; di < 2; di++)
		for (int dj=0; dj < 2; dj++)
			for (int dk=0; dk < 2; dk++)
				c[di][dj][dk] = s_RanVec[
					s_PermX[(i+di) & 255] ^ s_PermY[(j+dj) & 255] ^ s_PermZ[(k+dk) & 255]];
	return perlinInterp(c, u, v, w);
}

float Perlin::turb(const glm::vec3& p, int depth) const 
{
	float accum = 0;
	glm::vec3 temp_p = p;
	float weight = 1.0;
	for (int i = 0; i < depth; i++) {
		accum += weight*noise(temp_p);
		weight *= 0.5;
		temp_p *= 2;
	}
	return fabs(accum);
}
