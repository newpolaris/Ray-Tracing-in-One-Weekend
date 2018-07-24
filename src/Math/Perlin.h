#pragma once

#include <glm/glm.hpp>

namespace perlin
{
	void initialize();
	void shutdown();
}

class Perlin
{
public:
	
	Perlin();

	float noise(const glm::vec3& position) const;
};
