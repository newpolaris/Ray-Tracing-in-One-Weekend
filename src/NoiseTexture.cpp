#include "NoiseTexture.h"

NoiseTexture::NoiseTexture()
{
}

glm::vec3 NoiseTexture::value(float u, float v, const glm::vec3& position) const
{
	return glm::vec3(1.f)*m_Noise.turb(position);
}
