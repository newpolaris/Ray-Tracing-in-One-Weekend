#include <cmath>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

namespace Math
{
	inline float Random()
	{
		return drand48();
	}
}

//
// Solve intergral x^2, 0 < x < 2
//
// RESULT:
//
// Importance sampling: 2.668524
// Uniform sampling: 2.663355 
//

float f(float x) 
{
	return x*x;
}

float pdf(float x) 
{
	return 3 * x * x / 8;
}


int main()
{
	int n = 100000;
	float sum = 0.f;
	for (int i = 0; i < n; i++)
	{
		float x = std::pow(8 * Math::Random(), 1.f/3.f);
		sum += f(x) / pdf(x);
	}
	float ans = sum / n;
	printf("%f", ans);
}
