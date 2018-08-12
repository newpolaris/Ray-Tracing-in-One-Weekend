#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <Math/Random.h>
#include <Math/Sampling.h>

const int NumSamples = 1 << 19;

// [OneWeek] Uniform
float trace0()
{
	float sum = 0.f;
	for (int i = 0; i < NumSamples; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float x = cos(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
		float y = sin(2 * M_PI * r1) * 2 * sqrt(r2 * (1 - r2));
		float z = 1 - r2;
		sum += z * z * z / (1.f / (2.f * M_PI));
	}
	return sum / NumSamples;
}

// [OneWeek] Cosine weighted
float trace1()
{
	float sum = 0.f;
	for (int i = 0; i < NumSamples; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float x = cos(2 * M_PI * r1) * sqrt(r2);
		float y = sin(2 * M_PI * r1) * sqrt(r2);
		float z = sqrt(1 - r2);
		sum += z * z * z / (z / M_PI);
	}
	return sum / NumSamples;
}

// [PBRT] UniformSampleHemisphere
float trace2()
{
	float sum = 0.f;
	for (int i = 0; i < NumSamples; i++)
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
	return sum / NumSamples;
}


float trace3()
{
	float sum = 0.f;
	for (int i = 0; i < NumSamples; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		glm::vec3 v = Math::cosineSampleHemisphere(glm::vec2(r1, r2));

		sum += v.z*v.z*v.z / (v.z / M_PI);
	}
	return sum / NumSamples;
}

#define Test(a) \
	{ \
	auto k = trace##a();\
	printf("Trace%d = %1.6f, Err = %.6f\n", a, k, std::abs(M_PI/2 - k)); \
	} \

int main()
{
	printf("NumSamples = %d\n", NumSamples);
	printf("  PI/2 = %1.6f\n", M_PI/2);
	Test(0);
	Test(1);
	Test(2);
	Test(3);
}
