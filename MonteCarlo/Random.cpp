#include <iostream>
#include <functional>
#include <random>
#include <Math/RNG.h>

Float test(std::function<Float(void)> f)
{
	Float sum = 0u;
	auto k = 100;
	for (int i = 0; i < k; i++)
	{
		float t = f();
		sum += t;
		printf("%f\n", t);
	}
	return sum / k;
}

float RandMt19937()
{
	thread_local size_t id = std::hash<std::thread::id>{}(std::this_thread::get_id());
	thread_local std::mt19937 engine(std::random_device{}());
	engine.seed(id);
	std::uniform_real_distribution<Float> dist(0, 1);
	return dist(engine);
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
