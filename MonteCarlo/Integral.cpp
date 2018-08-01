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
// http://fracton.khu.ac.kr/~comp/monte/monte.html
//
// Solve intergral x^2, 0 < x < 2
// using Monte Carlo estimator for the integral
//
int main()
{
	auto f = [](float x) {
		return x*x;
	};
	int n = 100000;
	float ans = 0.f;
	float range = 2.f;
	float delta = (range / n);
	for (int i = 0; i < n; i++)
	{
		// intergral 0 to 2
		float x = 2 * Math::Random();
		ans += f(x);
	}
	ans *= delta;
	printf("%f", ans);
}
