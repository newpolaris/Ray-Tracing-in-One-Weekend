#include <iostream>
#include <functional>
#include <random>
#include "../src/Math/RNG.h"

Float test(std::function<Float(void)> f)
{
	Float sum = 0u;
	auto k = 100000;
	for (int i = 0; i < k; i++)
		sum += f();
	return sum / k;
}

int main()
{
	thread_local Math::RNG rng;
	printf("%f\n", test([&]{ return rng.UniformFloat(); }));

	thread_local std::mt19937 engine(std::random_device{}());
	// fix seed
	engine.seed(0);
	std::uniform_real_distribution<Float> dist(0, 1);
	printf("%f\n", test([&]{ return dist(engine);}));

	return 0;
}
