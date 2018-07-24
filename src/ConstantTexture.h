#pragma once

#include <Texture.h>

class ConstantTexture : public Texture
{
public:

    ConstantTexture();
    ConstantTexture(const glm::vec3& color);

    virtual glm::vec3 value(float u, float v, const glm::vec3& position) const;

private:

    glm::vec3 m_Color;
};
