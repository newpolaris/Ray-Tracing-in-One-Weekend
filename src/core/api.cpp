#include "api.h"
#include "paramset.h"
#include "light.h"
#include <map>
#include <stdio.h>
#include <core/error.h>

// API Global Variables
Options PbrtOptions;

// API Local Classes
constexpr int MaxTransforms = 2;
constexpr int StartTransformBits = 1 << 0;
constexpr int EndTransformBits = 1 << 1;
constexpr int AllTransformsBits = (1 << MaxTransforms) - 1;
struct TransformSet
{
};

struct RenderOptions {
    // RenderOptions Public Methods
    Integrator *MakeIntegrator() const;
    Scene *MakeScene();
    Camera *MakeCamera() const;

    // RenderOptions Public Data
    float transformStartTime = 0, transformEndTime = 1;
    std::string FilterName = "box";
    ParamSet FilterParams;
    std::string FilmName = "image";
    ParamSet FilmParams;
    std::string SamplerName = "halton";
    ParamSet SamplerParams;
    std::string AcceleratorName = "bvh";
    ParamSet AcceleratorParams;
    std::string IntegratorName = "path";
    ParamSet IntegratorParams;
    std::string CameraName = "perspective";
    ParamSet CameraParams;
    TransformSet CameraToWorld;
    std::map<std::string, std::shared_ptr<Medium>> namedMedia;
    std::vector<std::shared_ptr<class Light>> lights;
    std::vector<std::shared_ptr<class Primitive>> primitives;
    std::map<std::string, std::vector<std::shared_ptr<class Primitive>>> instances;
    std::vector<std::shared_ptr<class Primitive>> *currentInstance = nullptr;
    bool haveScatteringMedia = false;
};

struct GraphicsState {
    // Graphis State Tethods
    GraphicsState() {}
};

// API Static Data
enum class APIState { Uninitialized, OptionsBlock, WorldBlock };
static APIState currentApiState = APIState::Uninitialized;
static std::unique_ptr<RenderOptions> renderOptions;
static GraphicsState graphicsState;
static std::vector<GraphicsState> pushedGraphicsStates;
int catIndentCount = 0;

void pbrtInit(const Options &opt) {
    PbrtOptions = opt;
    // API Initialization
    if (currentApiState != APIState::Uninitialized)
        Error("pbrtInit() has already been called.");
    currentApiState = APIState::OptionsBlock;
    renderOptions.reset(new RenderOptions());
    graphicsState = GraphicsState();
    catIndentCount = 0;
}

void pbrtCleanup() {
}

void pbrtAccelerator(const std::string& name, const ParamSet& params)
{
    // VERIFY_OPTIONS("Accelerator");
    renderOptions->AcceleratorName = name;
    renderOptions->AcceleratorParams = params;
    if (PbrtOptions.cat || PbrtOptions.toPly)
    {
        printf("%*sAccelerator \"%s\" ", catIndentCount, "", name.c_str());
        params.Print(catIndentCount);
        printf("\n");
    }
}

