#include "Stochastic.h"

#include <iostream>

#include <assimp/cimport.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#include <gloperate/painter/TypedRenderTargetCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>

#include <gloperate/primitives/AdaptiveGrid.h>

#include <reflectionzeug/PropertyGroup.h>

#include "MasksTableGenerator.h"

#include "../AssimpLoader.h"
#include "../AssimpProcessing.h"
#include "../PolygonalDrawable.h"
#include "../PolygonalGeometry.h"
#include "../util.hpp"


using namespace gl;
using namespace glm;
using namespace globjects;

Stochastic::Stochastic(gloperate::ResourceManager & resourceManager)
:   Painter{resourceManager}
,   m_targetFramebufferCapability{new gloperate::TargetFramebufferCapability}
,   m_viewportCapability{new gloperate::ViewportCapability}
,   m_projectionCapability{new gloperate::PerspectiveProjectionCapability{m_viewportCapability}}
,   m_typedRenderTargetCapability{new gloperate::TypedRenderTargetCapability{}}
,   m_cameraCapability{new gloperate::CameraCapability{}}
,   m_timeCapability{new gloperate::VirtualTimeCapability}
,   m_transparency{0.5}
{
    m_timeCapability->setLoopDuration(20.0f * pi<float>());

    m_targetFramebufferCapability->changed.connect(this, &Stochastic::onTargetFramebufferChanged);

    addCapability(m_targetFramebufferCapability);
    addCapability(m_viewportCapability);
    addCapability(m_projectionCapability);
    addCapability(m_cameraCapability);
    addCapability(m_timeCapability);
    addCapability(m_typedRenderTargetCapability);
    
    setupPropertyGroup();
}

Stochastic::~Stochastic() = default;

reflectionzeug::PropertyGroup * Stochastic::propertyGroup() const
{
    return m_propertyGroup.get();
}

void Stochastic::setupPropertyGroup()
{
    m_propertyGroup = make_unique<reflectionzeug::PropertyGroup>();
    
    m_propertyGroup->addProperty<float>("transparency", this,
        &Stochastic::transparency, &Stochastic::setTransparency)->setOptions({
        { "minimum", 0.0f },
        { "maximum", 1.0f },
        { "step", 0.1f },
        { "precision", 1u }});
}

float Stochastic::transparency() const
{
    return m_transparency;
}

void Stochastic::setTransparency(float transparency)
{
    m_transparency = transparency;
}

void Stochastic::onInitialize()
{
    globjects::init();
    globjects::DebugMessage::enable();
    onTargetFramebufferChanged();

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
    setupMasksTexture();
}

void Stochastic::onPaint()
{
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

void Stochastic::onTargetFramebufferChanged()
{
    auto fbo = m_targetFramebufferCapability->framebuffer();

    if (!fbo)
        fbo = globjects::Framebuffer::defaultFBO();

    m_typedRenderTargetCapability->setRenderTarget(gloperate::RenderTargetType::Depth, fbo,
        GLenum::GL_DEPTH_ATTACHMENT, GLenum::GL_DEPTH_COMPONENT);
}

void Stochastic::setupFramebuffer()
{
    m_colorAttachment = Texture::createDefault(GL_TEXTURE_2D_MULTISAMPLE);
    m_depthAttachment = Texture::createDefault(GL_TEXTURE_2D_MULTISAMPLE);
    
    m_fbo = make_ref<Framebuffer>();

    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_colorAttachment);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_depthAttachment);
    
    updateFramebuffer();
    
    m_fbo->printStatus(true);
}

void Stochastic::setupProjection()
{
    static const auto zNear = 0.3f, zFar = 30.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));

    m_grid->setNearFar(zNear, zFar);
}

void Stochastic::setupDrawable()
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

void Stochastic::setupProgram()
{
    static const auto shaderPath = std::string{"data/transparency/stochastic"};
    
    const auto vertexShader = shaderPath + ".vert";
    const auto fragmentShader = shaderPath + ".frag";
    
    m_program = make_ref<Program>();
    m_program->attach(
        Shader::fromFile(GL_VERTEX_SHADER, vertexShader),
        Shader::fromFile(GL_FRAGMENT_SHADER, fragmentShader));
    
    const auto masksTextureLocation = m_program->getUniformLocation("masks");
    m_program->setUniform(masksTextureLocation, 0);
    
    m_transformLocation = m_program->getUniformLocation("transform");
    m_transparencyLocation = m_program->getUniformLocation("transparency");
}

void Stochastic::setupMasksTexture()
{
    static const auto numSamples = 8u;
    const auto table = MasksTableGenerator::generateDistributions(numSamples);
    
    m_masksTexture = Texture::createDefault(GL_TEXTURE_2D);
    m_masksTexture->image2D(0, GL_R8, table->size(), table->at(0).size(), 0, GL_RED, GL_UNSIGNED_BYTE, table->data());
}

void Stochastic::updateFramebuffer()
{
    static const auto numSamples = 4u;
    const auto width = m_viewportCapability->width(), height = m_viewportCapability->height();
    
    m_colorAttachment->image2DMultisample(numSamples, GL_RGBA8, width, height, GL_TRUE);
    m_depthAttachment->image2DMultisample(numSamples, GL_DEPTH_COMPONENT24, width, height, GL_TRUE);
}
