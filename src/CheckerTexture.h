#pragma once

#include <Types.h>
#include <Texture.h>

class CheckerTexture : public Texture
{
public:

    CheckerTexture();
    CheckerTexture(const TexturePtr& texOdd, const TexturePtr& texEven);

    virtual glm::vec3 value(float u, float v, const glm::vec3& position) const override;

private:

    TexturePtr m_Odd;
    TexturePtr m_Even;
};
