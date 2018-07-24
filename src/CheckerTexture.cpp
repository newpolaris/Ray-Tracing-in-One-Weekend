#include "CheckerTexture.h"

CheckerTexture::CheckerTexture()
{
}

CheckerTexture::CheckerTexture(const TexturePtr& texOdd, const TexturePtr& texEven)
    : m_Odd(texOdd)
    , m_Even(texEven)
{
}

glm::vec3 CheckerTexture::value(float u, float v, const glm::vec3& position) const
{
    auto signes = glm::sin(10.f*position);
    float sign = signes.x * signes.y * signes.z;
    return (sign < 0) ? m_Odd->value(u, v, position) : m_Even->value(u, v, position);
}
