#include <GL/glew.h>
#include <glfw3.h>

// GLM for matrix transformation
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <tools/gltools.hpp>
#include <tools/Profile.h>
#include <tools/imgui.h>
#include <tools/TCamera.h>

#include <GLType/GraphicsDevice.h>
#include <GLType/GraphicsData.h>
#include <GLType/OGLDevice.h>
#include <GLType/ProgramShader.h>
#include <GLType/GraphicsFramebuffer.h>

#include <GLType/OGLTexture.h>
#include <GLType/OGLCoreTexture.h>
#include <GLType/OGLCoreFramebuffer.h>

#include <GraphicsTypes.h>
#include "Mesh.h"
#include "BasicMesh.h"

#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <GameCore.h>
#include "Atmosphere.h"
#include <Math/Ray.h>
#include <Math/Random.h>
#include "Sphere.h"
#include "Camera.h"

enum ProfilerType { ProfilerTypeRender = 0 };

namespace 
{
    float s_CpuTick = 0.f;
    float s_GpuTick = 0.f;
}

struct SceneSettings
{
    bool bCPU = true;
    bool bProfile = true;
    bool bUiChanged = false;
    bool bResized = false;
    bool bUpdated = true;
	bool bChapman = true;
    float angle = 76.f;
    float intensity = 20.f;
    const int numSamples = 4;
};

static float Halton(int index, float base)
{
    float result = 0.0f;
    float f = 1.0f/base;
    float i = float(index);
    for (;;)
    {
        if (i <= 0.0f)
            break;

        result += f*fmodf(i, base);
        i = floorf(i/base);
        f = f/base;
    }
    return result;
}


static std::vector<glm::vec2> Halton2D(int size, int offset)
{
    std::vector<glm::vec2> s(size);
    for (int i = 0; i < size; i++)
    {
        s[i][0] = Halton(i + offset, 2.0f);
        s[i][1] = Halton(i + offset, 3.0f);
    }
    return s;
}

static std::vector<glm::vec4> Halton4D(int size, int offset)
{
    std::vector<glm::vec4> s(size);
    for (int i = 0; i < size; i++)
    {
        s[i][0] = Halton(i + offset, 2.0f);
        s[i][1] = Halton(i + offset, 3.0f);
        s[i][2] = Halton(i + offset, 5.0f);
        s[i][3] = Halton(i + offset, 7.0f);
    }
    return s;
}

glm::mat4 jitterProjMatrix(const glm::mat4& proj, int sampleCount, float jitterAASigma, float width, float height)
{
    // Per-frame jitter to camera for AA
    const int frameNum = sampleCount + 1; // Add 1 since otherwise first sample is an outlier

    float u1 = Halton(frameNum, 2.0f);
    float u2 = Halton(frameNum, 3.0f);

    // Gaussian sample
    float phi = 2.0f*glm::pi<float>()*u2;
    float r = jitterAASigma*sqrtf(-2.0f*log(std::max(u1, 1e-7f)));
    float x = r*cos(phi);
    float y = r*sin(phi);

    glm::mat4 ret = proj;
    ret[0].w += x*2.0f/width;
    ret[1].w += y*2.0f/height;

    return ret;
}

const float RAY_MIN = 0.0001f;
const float RAY_MAX = 1e8f;

glm::vec3 color(const HitableList& hitables, const Math::Ray& ray) 
{
	HitRecord rec = { 0 };

	if (hit(hitables.begin(), hitables.end(), ray, RAY_MIN, RAY_MAX, rec)) 
	{
		glm::vec3 target = rec.position + rec.normal + Math::randomUnitSphere();
		glm::vec3 dir = glm::normalize(target - rec.position);
		Math::Ray ray(rec.position, dir);
		return 0.5f * color(hitables, ray);
	}

	auto dir = glm::normalize(ray.direction());
	float t = 0.5f * dir.y + 1.f;
	return glm::mix(glm::vec3(1.f), glm::vec3(0.5f, 0.7f, 1.f), t);
}

void test(std::vector<glm::vec4>& image, int width, int height)
{
	const int NumSamples = 100;
	Camera camera;
	HitableList hitables;
	hitables.emplace_back(std::make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5f));
	hitables.emplace_back(std::make_shared<Sphere>(glm::vec3(0, -100.2f, -1), 100.0f));

	for (int y = height - 1; y >= 0; y--)
	for (int x = width - 1; x >= 0; x--)
	{
		glm::vec4 c(0.f);
		for (int s = 0; s < NumSamples; s++)
		{
			float u = float(x + Math::BaseRandom()) / width;
			float v = float(y + Math::BaseRandom()) / height;

			auto ray = camera.ray(u, v);
			c += glm::vec4(color(hitables, ray), 1.f);
		}
        image[y*width + x] = c / float(NumSamples);
	}
}

void writeToPPM(std::vector<glm::vec4>& image, int width, int height)
{
	FILE* pFile = fopen("result.ppm", "wb");
	fprintf(pFile, "P3\n");
	fprintf(pFile, "%d %d\n", width, height);
	fprintf(pFile, "255\n");
	for (int y = height - 1; y >= 0; y--)
	for (int x = 0; x < width; x++)
	{
		glm::vec3 source = glm::vec3(image[y*width + x]);
		glm::ivec3 color = glm::pow(source, glm::vec3(1.f/2.2f)) * 255.99f;
		fprintf(pFile, "%d %d %d\n", color.r, color.g, color.b);
	}
	fclose(pFile);
}

class RayTracer final : public gamecore::IGameApp
{
public:
	RayTracer() noexcept;
	virtual ~RayTracer() noexcept;

	virtual void startup() noexcept override;
	virtual void closeup() noexcept override;
	virtual void update() noexcept override;
    virtual void updateHUD() noexcept override;
	virtual void render() noexcept override;

	virtual void keyboardCallback(uint32_t c, bool bPressed) noexcept override;
	virtual void framesizeCallback(int32_t width, int32_t height) noexcept override;
	virtual void motionCallback(float xpos, float ypos, bool bPressed) noexcept override;
	virtual void mouseCallback(float xpos, float ypos, bool bPressed) noexcept override;

	GraphicsDevicePtr createDevice(const GraphicsDeviceDesc& desc) noexcept;

private:

    std::vector<glm::vec2> m_Samples;
    SceneSettings m_Settings;
	TCamera m_Camera;
    FullscreenTriangleMesh m_ScreenTraingle;
    ProgramShader m_SkyShader;
    ProgramShader m_BlitShader;
    GraphicsTexturePtr m_SkyColorTex;
    GraphicsTexturePtr m_ScreenColorTex;
    GraphicsFramebufferPtr m_ColorRenderTarget;
    GraphicsDevicePtr m_Device;
};

CREATE_APPLICATION(RayTracer);

RayTracer::RayTracer() noexcept
{
}

RayTracer::~RayTracer() noexcept
{
}

void RayTracer::startup() noexcept
{
	profiler::initialize();

	m_Camera.setViewParams(glm::vec3(2.0f, 5.0f, 15.0f), glm::vec3(2.0f, 0.0f, 0.0f));
	m_Camera.setMoveCoefficient(0.35f);

	GraphicsDeviceDesc deviceDesc;
#if __APPLE__
	deviceDesc.setDeviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGL);
#else
	deviceDesc.setDeviceType(GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore);
#endif
	m_Device = createDevice(deviceDesc);
	assert(m_Device);

	m_SkyShader.setDevice(m_Device);
	m_SkyShader.initialize();
	m_SkyShader.addShader(GL_VERTEX_SHADER, "Scattering.Vertex");
	m_SkyShader.addShader(GL_FRAGMENT_SHADER, "Scattering.Fragment");
	m_SkyShader.link();

	m_BlitShader.setDevice(m_Device);
	m_BlitShader.initialize();
	m_BlitShader.addShader(GL_VERTEX_SHADER, "BlitTexture.Vertex");
	m_BlitShader.addShader(GL_FRAGMENT_SHADER, "BlitTexture.Fragment");
	m_BlitShader.link();

    m_ScreenTraingle.create();

    m_Samples = Halton2D(m_Settings.numSamples, 0);
}

void RayTracer::closeup() noexcept
{
    m_ScreenTraingle.destroy();
	profiler::shutdown();
}

void RayTracer::update() noexcept
{
    bool bCameraUpdated = m_Camera.update();

    static int32_t preWidth = 0;
    static int32_t preHeight = 0;

    int32_t width = getFrameWidth();
    int32_t height = getFrameHeight();
    bool bResized = false;
    if (preWidth != width || preHeight != height)
    {
        preWidth = width, preHeight = height;
        bResized = true;
    }
    m_Settings.bUpdated = (m_Settings.bUiChanged || bCameraUpdated || bResized);
    if (m_Settings.bUpdated && m_Settings.bCPU)
    {
        const float angle = glm::radians(m_Settings.angle);
        std::vector<glm::vec4> image(width*height, glm::vec4(0.f));
        glm::vec3 sunDir = glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle));

		profiler::start(ProfilerTypeRender);

		test(image, width, height);
		writeToPPM(image, width, height);

		profiler::stop(ProfilerTypeRender);
		profiler::tick(ProfilerTypeRender, s_CpuTick, s_GpuTick);

        GraphicsTextureDesc colorDesc;
        colorDesc.setWidth(width);
        colorDesc.setHeight(height);
        colorDesc.setFormat(gli::FORMAT_RGBA32_SFLOAT_PACK32);
        colorDesc.setStream((uint8_t*)image.data());
        colorDesc.setStreamSize(width*height*sizeof(glm::vec4));
        m_SkyColorTex = m_Device->createTexture(colorDesc);
    }
}

void RayTracer::updateHUD() noexcept
{
    bool bUpdated = false;
    float width = (float)getWindowWidth(), height = (float)getWindowHeight();

    ImGui::SetNextWindowPos(
        ImVec2(width - width / 4.f - 10.f, 10.f),
        ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Settings",
        NULL,
        ImVec2(width / 4.0f, height - 20.0f),
        ImGuiWindowFlags_AlwaysAutoResize);
    bUpdated |= ImGui::Checkbox("Mode CPU", &m_Settings.bCPU);
    bUpdated |= ImGui::Checkbox("Always redraw", &m_Settings.bProfile);
	bUpdated |= ImGui::Checkbox("Use chapman approximation", &m_Settings.bChapman);
    bUpdated |= ImGui::SliderFloat("Sun Angle", &m_Settings.angle, 0.f, 120.f);
    bUpdated |= ImGui::SliderFloat("Sun Intensity", &m_Settings.intensity, 10.f, 50.f);
    ImGui::Text("CPU %s: %10.5f ms\n", "main", s_CpuTick);
    ImGui::Text("GPU %s: %10.5f ms\n", "main", s_GpuTick);
    ImGui::PushItemWidth(180.0f);
    ImGui::Indent();
    ImGui::Unindent();
    ImGui::End();

    m_Settings.bUiChanged = bUpdated;
}

void RayTracer::render() noexcept
{
	gamecore::closeApplication();
	return;

    bool bUpdate = m_Settings.bProfile || m_Settings.bUpdated;

    if (!m_Settings.bCPU && bUpdate)
    {
        auto& desc = m_ScreenColorTex->getGraphicsTextureDesc();
        m_Device->setFramebuffer(m_ColorRenderTarget);
        GLenum clearFlag = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
        glViewport(0, 0, desc.getWidth(), desc.getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepthf(1.0f);
        glClear(clearFlag);

        glDisable(GL_DEPTH_TEST);
        float angle = glm::radians(m_Settings.angle);
        float halfFov = m_Camera.getFov()/2.f;
		glm::vec2 resolution(desc.getWidth(), desc.getHeight());
        glm::vec3 sunDir = glm::vec3(0.0f, glm::cos(angle), -glm::sin(angle));
        m_SkyShader.bind();
        m_SkyShader.setUniform("uChapman", m_Settings.bChapman);
        m_SkyShader.setUniform("uEarthRadius", 6360e3f);
        m_SkyShader.setUniform("uAtmosphereRadius", 6420e3f);
		m_SkyShader.setUniform("uInvResolution", 1.f/resolution);
        m_SkyShader.setUniform("uEarthCenter", glm::vec3(0.f));
        m_SkyShader.setUniform("uSunDir", sunDir);
        m_SkyShader.setUniform("uAspect", m_Camera.getAspect());
        m_SkyShader.setUniform("uAngle", glm::tan(glm::radians(halfFov)));
        m_SkyShader.setUniform("uSamples", m_Samples.data(), 4);
        m_SkyShader.setUniform("uSunIntensity", glm::vec3(m_Settings.intensity));
        m_ScreenTraingle.draw();
        glEnable(GL_DEPTH_TEST);
    }
    // Tone mapping
    {
        GraphicsTexturePtr target = m_ScreenColorTex;
        if (m_Settings.bCPU && m_SkyColorTex) 
            target = m_SkyColorTex;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, getFrameWidth(), getFrameHeight());

        glDisable(GL_DEPTH_TEST);
        m_BlitShader.bind();
        m_BlitShader.bindTexture("uTexSource", target, 0);
        m_ScreenTraingle.draw();
        glEnable(GL_DEPTH_TEST);
    }
}

void RayTracer::keyboardCallback(uint32_t key, bool isPressed) noexcept
{
	switch (key)
	{
	case GLFW_KEY_UP:
		m_Camera.keyboardHandler(MOVE_FORWARD, isPressed);
		break;

	case GLFW_KEY_DOWN:
		m_Camera.keyboardHandler(MOVE_BACKWARD, isPressed);
		break;

	case GLFW_KEY_LEFT:
		m_Camera.keyboardHandler(MOVE_LEFT, isPressed);
		break;

	case GLFW_KEY_RIGHT:
		m_Camera.keyboardHandler(MOVE_RIGHT, isPressed);
		break;
	}
}

void RayTracer::framesizeCallback(int32_t width, int32_t height) noexcept
{
	float aspectRatio = (float)width/height;
	m_Camera.setProjectionParams(45.0f, aspectRatio, 0.1f, 100.0f);

    GraphicsTextureDesc colorDesc;
    colorDesc.setWidth(width);
    colorDesc.setHeight(height);
    colorDesc.setFormat(gli::FORMAT_RGBA16_SFLOAT_PACK16);
    m_ScreenColorTex = m_Device->createTexture(colorDesc);

    GraphicsTextureDesc depthDesc;
    depthDesc.setWidth(width);
    depthDesc.setHeight(height);
    depthDesc.setFormat(gli::FORMAT_D24_UNORM_S8_UINT_PACK32);
    auto depthTex = m_Device->createTexture(depthDesc);

    GraphicsFramebufferDesc desc;  
    desc.addComponent(GraphicsAttachmentBinding(m_ScreenColorTex, GL_COLOR_ATTACHMENT0));
    desc.addComponent(GraphicsAttachmentBinding(depthTex, GL_DEPTH_ATTACHMENT));
    
    m_ColorRenderTarget = m_Device->createFramebuffer(desc);;
}

void RayTracer::motionCallback(float xpos, float ypos, bool bPressed) noexcept
{
	const bool mouseOverGui = ImGui::MouseOverArea();
	if (!mouseOverGui && bPressed) m_Camera.motionHandler(int(xpos), int(ypos), false);    
}

void RayTracer::mouseCallback(float xpos, float ypos, bool bPressed) noexcept
{
	const bool mouseOverGui = ImGui::MouseOverArea();
	if (!mouseOverGui && bPressed) m_Camera.motionHandler(int(xpos), int(ypos), true); 
}

GraphicsDevicePtr RayTracer::createDevice(const GraphicsDeviceDesc& desc) noexcept
{
	GraphicsDeviceType deviceType = desc.getDeviceType();

#if __APPLE__
	assert(deviceType != GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore);
#endif

	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore)
    {
        auto device = std::make_shared<OGLDevice>();
        if (device->create(desc))
            return device;
        return nullptr;
    }
    return nullptr;
}
