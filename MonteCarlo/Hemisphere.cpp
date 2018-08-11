#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <Math/Random.h>

const int N = 1000000;

float trace0()
{
	float sum = 0.f;
	for (int i = 0; i < N; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float x = cos(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
		float y = sin(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
		float z = 1 - r2;
		sum += z * z * z / (1.f / (2.f * M_PI));
	}
	return sum / N;
}

float trace1()
{
	float sum = 0.f;
	for (int i = 0; i < N; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float x = cos(2 * M_PI * r1) * sqrt(r2);
		float y = sin(2 * M_PI * r1) * sqrt(r2);
		float z = sqrt(1 - r2);
		sum += z * z * z / (z / M_PI);
	}
	return sum / N;
}

// [PBRT] UniformSampleHemisphere
float trace2()
{
	float sum = 0.f;
	for (int i = 0; i < N; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float z = r1;
		float r = sqrt(fmax(0, 1.f - z * z));
		float phi = 2 * M_PI * r2;
		float x = r * cos(phi);
		float y = r * sin(phi);

		sum += z * z * z / (1.f / (2.f * M_PI));
	}
	return sum / N;
}

// [PBRT] UniformSampleDisk
glm::vec2 uniformSampleDisk(const glm::vec2& u)
{
	float r = sqrt(u[0]);
	float theta = 2 * M_PI * u[1];
	return glm::vec2(r * cos(theta), r * sin(theta));
}

// [PBRT] UniformSampleDisk
glm::vec2 concentricSampleDisk(const glm::vec2& u)
{
	// Map uniform ramdom numbers to [-1, 1]^2
	glm::vec2 uOffset = 2.f * u - glm::vec2(1.f);
	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0)
		return glm::vec2(0.f);
	// Apply concentric mapping to point
	float theta, r;
	if (fabs(uOffset.x) > abs(uOffset.y))
	{
		r = uOffset.x;
		theta = M_PI/4 * (uOffset.y / uOffset.x);
	}
	else
	{
		r = uOffset.y;
		theta = M_PI/2 - M_PI/4 * (uOffset.x / uOffset.y);
	}
	return r * glm::vec2(cos(theta), sin(theta));
}

// [PBRT] CosineSampleHemisphere
glm::vec3 cosineSampleHemisphere(const glm::vec2& u)
{
	glm::vec2 d = concentricSampleDisk(u);
	float z = sqrt(fmax(0, 1 - d.x*d.x - d.y*d.y));
	return glm::vec3(d.x, d.y, z);
}

float trace3()
{
	float sum = 0.f;
	for (int i = 0; i < N; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		glm::vec3 v = cosineSampleHemisphere(glm::vec2(r1, r2));

		sum += v.z*v.z*v.z / (v.z / M_PI);
	}
	return sum / N;
}

int main()
{
	printf("  PI/2 = %1.6f\n", M_PI/2);
	printf("Trace0 = %1.6f\n", trace0());
	printf("Trace1 = %1.6f\n", trace1());
	printf("Trace2 = %1.6f\n", trace2());
	printf("Trace3 = %1.6f\n", trace3());
}
