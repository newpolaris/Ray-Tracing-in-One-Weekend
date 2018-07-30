#include "ImageTexture.h"
#include <algorithm>
#include <tools/stb_image.h>

ImageTexture::ImageTexture()
{
}

ImageTexture::ImageTexture(const std::string& filename)
{
	int width, height, nrComponents, numComponent = 3;
    stbi_uc* imagedata = stbi_load(filename.c_str(), &width, &height, &nrComponents, numComponent);
    assert(imagedata != nullptr);
	std::vector<std::uint8_t> data(imagedata, imagedata + width*height*numComponent);
	stbi_image_free(imagedata);

	m_Data = std::move(data);
	m_Width = width;
	m_Height = height;
}

glm::vec3 ImageTexture::value(float u, float v, const glm::vec3& position) const
{
	int i = int(u * float(m_Width - 1));
	int j = int((1.f - v) * float(m_Height - 1));
	i = std::max(0, i);
	j = std::max(0, j);
	i = std::min(i, m_Width - 1);
	j = std::min(j, m_Height - 1);
	int pos = 3 * (i + m_Width*j);
	float r = int(m_Data[pos + 0]) / 255.f;
	float g = int(m_Data[pos + 1]) / 255.f;
	float b = int(m_Data[pos + 2]) / 255.f;
	return glm::vec3(r, g, b);
}
