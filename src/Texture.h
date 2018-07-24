#pragma once

#include <glm/glm.hpp>

class Texture 
{
public:

    virtual glm::vec3 value(float u, float v, const glm::vec3& position) const = 0;
};