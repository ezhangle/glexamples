#include "ScreenDoor.h"

#include <iostream>

#include <assimp/cimport.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <glbinding/gl/boolean.h>
#include <glbinding/gl/enum.h>
#include <glbinding/gl/bitfield.h>

#include <globjects/globjects.h>
#include <globjects/logging.h>
#include <globjects/Framebuffer.h>
#include <globjects/DebugMessage.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>

#include <gloperate/base/RenderTargetType.h>

#include <gloperate/painter/TargetFramebufferCapability.h>
#include <gloperate/painter/ViewportCapability.h>
#include <gloperate/painter/PerspectiveProjectionCapability.h>
#include <gloperate/painter/CameraCapability.h>

#include <gloperate/primitives/AdaptiveGrid.h>

#include <reflectionzeug/PropertyGroup.h>
#include <widgetzeug/make_unique.hpp>

#include "../AssimpLoader.h"
#include "../AssimpProcessing.h"
#include "../PolygonalDrawable.h"
#include "../PolygonalGeometry.h"


using namespace gl;
using namespace glm;
using namespace globjects;

using widgetzeug::make_unique;

ScreenDoor::ScreenDoor(gloperate::ResourceManager & resourceManager)
:   Painter(resourceManager)
,   m_targetFramebufferCapability{addCapability(make_unique<gloperate::TargetFramebufferCapability>())}
,   m_viewportCapability{addCapability(make_unique<gloperate::ViewportCapability>())}
,   m_projectionCapability{addCapability(make_unique<gloperate::PerspectiveProjectionCapability>(m_viewportCapability))}
,   m_cameraCapability{addCapability(make_unique<gloperate::CameraCapability>())}
,   m_multisampling{false}
,   m_multisamplingChanged{false}
,   m_transparency{0.5}
{    
    setupPropertyGroup();
}

ScreenDoor::~ScreenDoor() = default;

void ScreenDoor::setupPropertyGroup()
{
    addProperty<bool>("multisampling", this,
        &ScreenDoor::multisampling, &ScreenDoor::setMultisampling);
    
    addProperty<float>("transparency", this,
        &ScreenDoor::transparency, &ScreenDoor::setTransparency)->setOptions({
        { "minimum", 0.0f },
        { "maximum", 1.0f },
        { "step", 0.1f },
        { "precision", 1u }});
}

bool ScreenDoor::multisampling() const
{
    return m_multisampling;
}

void ScreenDoor::setMultisampling(bool b)
{
    m_multisamplingChanged = m_multisampling != b;
    m_multisampling = b;
}

float ScreenDoor::transparency() const
{
    return m_transparency;
}

void ScreenDoor::setTransparency(float transparency)
{
    m_transparency = transparency;
}

void ScreenDoor::onInitialize()
{
    globjects::init();
    globjects::DebugMessage::enable();

#ifdef __APPLE__
    Shader::clearGlobalReplacements();
    Shader::globalReplace("#version 140", "#version 150");

    debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

    m_grid = make_ref<gloperate::AdaptiveGrid>();
    m_grid->setColor({0.6f, 0.6f, 0.6f});

    setupDrawable();
    setupProgram();
    setupProjection();
    setupFramebuffer();
}

void ScreenDoor::onPaint()
{
    if (m_multisamplingChanged)
    {
        m_multisamplingChanged = false;
        setupProgram();
        setupFramebuffer();
    }
    
    if (m_viewportCapability->hasChanged())
    {
        glViewport(
            m_viewportCapability->x(),
            m_viewportCapability->y(),
            m_viewportCapability->width(),
            m_viewportCapability->height());

        m_viewportCapability->setChanged(false);
        
        updateFramebuffer();
    }

    m_fbo->bind(GL_FRAMEBUFFER);
    m_fbo->clearBuffer(GL_COLOR, 0, glm::vec4{0.85f, 0.87f, 0.91f, 1.0f});
    m_fbo->clearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);

    const auto transform = m_projectionCapability->projection() * m_cameraCapability->view();
    const auto eye = m_cameraCapability->eye();

    m_grid->update(eye, transform);
    m_grid->draw();
    
    glEnable(GL_MIN_SAMPLE_SHADING_VALUE);
    glMinSampleShading(1.0);
    
    m_program->use();
    m_program->setUniform(m_transformLocation, transform);
    
    for (auto i = 0u; i < m_drawables.size(); ++i)
    {
        m_program->setUniform(m_transparencyLocation, i % 2 == 0 ? m_transparency : 1.0f);
        m_drawables[i]->draw();
    }
    
    m_program->release();
    
    glDisable(GL_MIN_SAMPLE_SHADING_VALUE);
    glMinSampleShading(0.0);

    Framebuffer::unbind(GL_FRAMEBUFFER);
    
    const auto rect = std::array<gl::GLint, 4>{{
        m_viewportCapability->x(),
        m_viewportCapability->y(),
        m_viewportCapability->width(),
        m_viewportCapability->height()}};
    
    auto targetfbo = m_targetFramebufferCapability->framebuffer();
    auto drawBuffer = GL_COLOR_ATTACHMENT0;
    
    if (!targetfbo)
    {
        targetfbo = globjects::Framebuffer::defaultFBO();
        drawBuffer = GL_BACK_LEFT;
    }
    
    m_fbo->blit(GL_COLOR_ATTACHMENT0, rect, targetfbo, drawBuffer, rect,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void ScreenDoor::setupFramebuffer()
{
    const auto textureTarget = m_multisampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    
    m_colorAttachment = Texture::createDefault(textureTarget);
    m_depthAttachment = Texture::createDefault(textureTarget);
    
    m_fbo = make_ref<Framebuffer>();

    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_colorAttachment);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_depthAttachment);
    
    updateFramebuffer();
    
    m_fbo->printStatus(true);
}

void ScreenDoor::setupProjection()
{
    static const auto zNear = 0.3f, zFar = 30.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));

    m_grid->setNearFar(zNear, zFar);
}

void ScreenDoor::setupDrawable()
{
    auto assimpLoader = AssimpLoader{};
    const auto scene = assimpLoader.load("data/transparency/transparency_scene.obj", {});

    if (!scene)
    {
        std::cout << "Could not load file" << std::endl;
        return;
    }

    const auto geometries = AssimpProcessing::convertToGeometries(scene);

    aiReleaseImport(scene);
    
    for (const auto & geometry : geometries)
        m_drawables.push_back(make_unique<PolygonalDrawable>(geometry));
}

void ScreenDoor::setupProgram()
{
    static const auto shaderPath = std::string{"data/transparency/"};
    const auto shaderName = m_multisampling ? "screendoor_multisample" : "screendoor";
    
    const auto vertexShader = shaderPath + shaderName + ".vert";
    const auto fragmentShader = shaderPath + shaderName + ".frag";
    
    m_program = make_ref<Program>();
    m_program->attach(
        Shader::fromFile(GL_VERTEX_SHADER, vertexShader),
        Shader::fromFile(GL_FRAGMENT_SHADER, fragmentShader));
    
    m_transformLocation = m_program->getUniformLocation("transform");
    m_transparencyLocation = m_program->getUniformLocation("transparency");
}

void ScreenDoor::updateFramebuffer()
{
    static const auto numSamples = 4u;
    const auto width = m_viewportCapability->width(), height = m_viewportCapability->height();
    
    if (m_multisampling)
    {
        m_colorAttachment->image2DMultisample(numSamples, GL_RGBA8, width, height, GL_TRUE);
        m_depthAttachment->image2DMultisample(numSamples, GL_DEPTH_COMPONENT, width, height, GL_TRUE);
    }
    else
    {
        m_colorAttachment->image2D(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        m_depthAttachment->image2D(0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    }
}
