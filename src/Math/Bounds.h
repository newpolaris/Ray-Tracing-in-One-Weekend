#pragma once

namespace Math
{

class Bounds2
{
public:
    Bounds2() {}
};

inline Bounds2* begin(const Bounds2)
{}

inline Bounds2* end(const Bounds2)
{}
static void k()
{
    for (auto k : Bounds2())
    {
    }
}

}
