#include <math.h>
#include <stdlib.h>
#include <iostream>

namespace Math
{
	inline float Random()
	{
		return drand48();
	}
}

void Run()
{
	int64_t inside_circle = 0;
	int64_t inside_circle_stratified = 0;
	int sqrt_N = 10000;
	for (int i = 0; i < sqrt_N; i++)
	{
		for (int j = 0; j < sqrt_N; j++)
		{
			float x = 2*Math::Random() - 1;
			float y = 2*Math::Random() - 1;
			if (x*x + y*y < 1.f)
				inside_circle++;

			x = 2 * ((i + Math::Random()) / sqrt_N) - 1;
			y = 2 * ((j + Math::Random()) / sqrt_N) - 1;
			if (x*x + y*y < 1.f)
				inside_circle_stratified++;
		}
	}
	double RPI = 4*double(inside_circle) / (sqrt_N * sqrt_N);
	std::cout << "Regular Estimate of PI = " << RPI << std::endl;

	double SPI = 4*double(inside_circle_stratified) / (sqrt_N * sqrt_N);
	std::cout << "Stratified Estimate of PI = " << SPI << std::endl;
}

int main()
{
	Run();
	return 0;
}
