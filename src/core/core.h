#pragma once

#if defined(_WIN32) || defined(_WIN64)
  #define IS_WINDOWS
#endif

#include <memory>
#include <glog/logging.h>
#include <vector>
#include <iostream>
#include <stdint.h>
#if defined(IS_MSVC)
#include <float.h>
#include <intrin.h>
#pragma warning(disable : 4305)  // double constant assigned to float
#pragma warning(disable : 4244)  // int -> float conversion
#pragma warning(disable : 4843)  // double -> float conversion
#endif

// Global Macros
#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

using Float = float;

class Scene;
class Integrator;
class Camera;
struct CameraSample;
class ProjectiveCamera;
class Sampler;
class Filter;
class Film;
class FilmTile;
class BxDF;
class BRDF;
class BTDF;
class BSDF;
class Material;
template <typename T> 
class Texture;
class Medium;
class MediumInteraction;
struct MediumInterface;
class ParamSet;
template <typename T>
struct ParamSetItem;
#ifdef PBRT_FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
#endif  // PBRT_FLOAT_AS_DOUBLE
struct Options
{
    Options() {
        cropWindow[0][0] = 0;
        cropWindow[0][1] = 1;
        cropWindow[1][0] = 0;
        cropWindow[1][1] = 1;
    }
    int nThreads = 0;
    bool quickRender = false;
    bool quiet = false;
    bool cat = false, toPly = false;
    std::string imageFile;
    // x0, x1, y0, y1
    float cropWindow[2][2];
};

extern Options PbrtOptions;
class TextureParams;
