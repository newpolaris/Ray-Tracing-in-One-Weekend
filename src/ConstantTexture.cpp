#include "ConstantTexture.h"

ConstantTexture::ConstantTexture()
{
}

 ConstantTexture::ConstantTexture(const glm::vec3& color)
     : m_Color(color)
 {
 }

glm::vec3 ConstantTexture::value(float /*u*/, float /*v*/, const glm::vec3& /*position*/) const
{
    return m_Color;
}
