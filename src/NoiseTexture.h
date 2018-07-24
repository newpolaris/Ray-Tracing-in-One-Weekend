#pragma once

#include <Texture.h>
#include <Math/Perlin.h>

class NoiseTexture : public Texture
{
public:

	NoiseTexture();

	virtual glm::vec3 value(float u, float v, const glm::vec3& position) const;

	Perlin m_Noise;
};
