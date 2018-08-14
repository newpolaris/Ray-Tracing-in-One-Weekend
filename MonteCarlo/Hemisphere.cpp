#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include <Math/Random.h>
#include <Math/Sampling.h>

const int NumSamples = 1 << 15;

// Uniform hemipshere
// https://qiita.com/mebiusbox2/items/5a388ef4d5089568a529
float trace0()
{
	float sum = 0.f;
	for (int i = 0; i < NumSamples; i++)
	{
		float r1 = Math::BaseRandom();
		float r2 = Math::BaseRandom();
		float x = cos(2 * M_PI * r1) * sqrt(r2 * (2 - r2));
		float y = sin(2 * M_PI * r1) * sqrt(r2 * (2 - r2));
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
		auto v = Math::randomCosineDirection();
		auto pdf = Math::randomCosineDirectionPdf(v.z);
		sum += v.z * v.z * v.z / pdf;
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
		glm::vec3 v = Math::uniformSampleHemisphere(glm::vec2(r1, r2));
		sum += v.z * v.z * v.z / (1.f / (2.f * M_PI));
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
