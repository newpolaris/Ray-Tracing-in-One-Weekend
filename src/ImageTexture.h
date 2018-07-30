#pragma once

#include <string>
#include <vector>
#include <Texture.h>

class ImageTexture : public Texture
{
public:

	ImageTexture();
	ImageTexture(const std::string& filename);

	virtual glm::vec3 value(float u, float v, const glm::vec3& position) const;

private:

	std::vector<std::uint8_t> m_Data;
	int m_Width, m_Height;
};
