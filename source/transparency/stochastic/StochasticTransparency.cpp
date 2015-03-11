#include "StochasticTransparency.h"

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
#include <gloperate/painter/TypedRenderTargetCapability.h>
#include <gloperate/painter/VirtualTimeCapability.h>

#include <gloperate/primitives/AdaptiveGrid.h>
#include <gloperate/primitives/ScreenAlignedQuad.h>

#include <reflectionzeug/PropertyGroup.h>

#include "MasksTableGenerator.h"
#include "StochasticTransparencyOptions.h"

#include "../AssimpLoader.h"
#include "../AssimpProcessing.h"
#include "../PolygonalDrawable.h"
#include "../PolygonalGeometry.h"
#include "../util.hpp"


using namespace gl;
using namespace glm;
using namespace globjects;

StochasticTransparency::StochasticTransparency(gloperate::ResourceManager & resourceManager)
:   Painter{resourceManager}
,   m_targetFramebufferCapability{new gloperate::TargetFramebufferCapability}
,   m_viewportCapability{new gloperate::ViewportCapability}
,   m_projectionCapability{new gloperate::PerspectiveProjectionCapability{m_viewportCapability}}
,   m_typedRenderTargetCapability{new gloperate::TypedRenderTargetCapability{}}
,   m_cameraCapability{new gloperate::CameraCapability{}}
,   m_timeCapability{new gloperate::VirtualTimeCapability}
,   m_options{make_unique<StochasticTransparencyOptions>()}
{
    m_timeCapability->setLoopDuration(20.0f * pi<float>());

    m_targetFramebufferCapability->changed.connect(this, &StochasticTransparency::onTargetFramebufferChanged);

    addCapability(m_targetFramebufferCapability);
    addCapability(m_viewportCapability);
    addCapability(m_projectionCapability);
    addCapability(m_cameraCapability);
    addCapability(m_timeCapability);
    addCapability(m_typedRenderTargetCapability);
}

StochasticTransparency::~StochasticTransparency() = default;

reflectionzeug::PropertyGroup * StochasticTransparency::properties() const
{
    return m_options.get();
}

void StochasticTransparency::onInitialize()
{
    globjects::init();
    globjects::DebugMessage::enable();
    
    onTargetFramebufferChanged();

#ifdef __APPLE__
    Shader::clearGlobalReplacements();
    Shader::globalReplace("#version 140", "#version 150");

    debug() << "Using global OS X shader replacement '#version 140' -> '#version 150'" << std::endl;
#endif

    m_options->initGL();
    
    m_grid = make_ref<gloperate::AdaptiveGrid>();
    m_grid->setColor({0.6f, 0.6f, 0.6f});
    
    setupPrograms();
    setupProjection();
    setupFramebuffer();
    setupMasksTexture();
    setupDrawable();
}

void StochasticTransparency::onPaint()
{
    if (m_viewportCapability->hasChanged())
    {
        glViewport(
            m_viewportCapability->x(),
            m_viewportCapability->y(),
            m_viewportCapability->width(),
            m_viewportCapability->height());

        m_viewportCapability->setChanged(false);
        
        const auto viewport = glm::vec2{m_viewportCapability->width(), m_viewportCapability->height()};
        m_alphaToCoverageProgram->setUniform("viewport", viewport);
        
        updateFramebuffer();
    }
    
    if (m_options->numSamplesChanged())
        updateNumSamples();
    
    clearBuffers();
    updateUniforms();
    
    renderOpaqueGeometry();
    renderTransparentGeometry();
    composite();
    
    Framebuffer::unbind(GL_FRAMEBUFFER);
}

void StochasticTransparency::onTargetFramebufferChanged()
{
    auto fbo = m_targetFramebufferCapability->framebuffer();

    if (!fbo)
        fbo = globjects::Framebuffer::defaultFBO();

    m_typedRenderTargetCapability->setRenderTarget(gloperate::RenderTargetType::Depth, fbo,
        GLenum::GL_DEPTH_ATTACHMENT, GLenum::GL_DEPTH_COMPONENT);
}

void StochasticTransparency::setupFramebuffer()
{
    m_opaqueColorAttachment = make_ref<Texture>(GL_TEXTURE_2D_MULTISAMPLE);
    m_transparentColorAttachment = make_ref<Texture>(GL_TEXTURE_2D_MULTISAMPLE);
    m_totalAlphaAttachment = make_ref<Texture>(GL_TEXTURE_2D_MULTISAMPLE);
    m_depthAttachment = make_ref<Texture>(GL_TEXTURE_2D_MULTISAMPLE);
    
    updateFramebuffer();
    
    m_fbo = make_ref<Framebuffer>();
    
    m_fbo->attachTexture(kOpaqueColorAttachment, m_opaqueColorAttachment);
    m_fbo->attachTexture(kTransparentColorAttachment, m_transparentColorAttachment);
    m_fbo->attachTexture(kTotalAlphaAttachment, m_totalAlphaAttachment);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_depthAttachment);

    m_fbo->printStatus(true);
}

void StochasticTransparency::setupProjection()
{
    static const auto zNear = 0.3f, zFar = 30.f, fovy = 50.f;

    m_projectionCapability->setZNear(zNear);
    m_projectionCapability->setZFar(zFar);
    m_projectionCapability->setFovy(radians(fovy));

    m_grid->setNearFar(zNear, zFar);
}

void StochasticTransparency::setupDrawable()
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

void StochasticTransparency::setupPrograms()
{
    static const auto totalAlphaShaders = "total_alpha";
    static const auto alphaToCoverageShaders = "alpha_to_coverage";
    static const auto transparentColorsShaders = "transparent_colors";
    static const auto compositingShaders = "compositing";
    
    const auto initProgram = [] (globjects::ref_ptr<globjects::Program> & program, const char * shaders)
    {
        static const auto shaderPath = std::string{"data/transparency/"};
        
        program = make_ref<Program>();
        program->attach(
            Shader::fromFile(GL_VERTEX_SHADER, shaderPath + shaders + ".vert"),
            Shader::fromFile(GL_FRAGMENT_SHADER, shaderPath + shaders + ".frag"));
    };
    
    initProgram(m_totalAlphaProgram, totalAlphaShaders);
    initProgram(m_alphaToCoverageProgram, alphaToCoverageShaders);
    initProgram(m_colorAccumulationProgram, transparentColorsShaders);
    initProgram(m_compositingProgram, compositingShaders);
    
    m_alphaToCoverageProgram->setUniform("masksTexture", 0);
    
    updateNumSamplesUniforms();
    
    const auto opaqueColorLocation = m_compositingProgram->getUniformLocation("opaqueColorTexture");
    const auto totalAlphaLocation = m_compositingProgram->getUniformLocation("totalAlphaTexture");
    const auto transparentColorLocation = m_compositingProgram->getUniformLocation("transparentColorTexture");
    
    m_compositingProgram->setUniform(opaqueColorLocation, 0);
    m_compositingProgram->setUniform(totalAlphaLocation, 1);
    m_compositingProgram->setUniform(transparentColorLocation, 2);
    
    m_compositingQuad = make_ref<gloperate::ScreenAlignedQuad>(m_compositingProgram);
}

void StochasticTransparency::setupMasksTexture()
{
    static const auto numSamples = m_options->numSamples();
    const auto table = MasksTableGenerator::generateDistributions(numSamples);
    
    m_masksTexture = Texture::createDefault(GL_TEXTURE_2D);
    m_masksTexture->image2D(0, GL_R32UI, table->at(0).size(), table->size(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, table->data());
}

void StochasticTransparency::updateFramebuffer()
{
    const auto numSamples = m_options->numSamples();
    const auto size = glm::ivec2{m_viewportCapability->width(), m_viewportCapability->height()};
    
    m_opaqueColorAttachment->image2DMultisample(numSamples, GL_RGBA8, size, GL_FALSE);
    m_transparentColorAttachment->image2DMultisample(numSamples, GL_RGBA32F, size, GL_FALSE);
    m_totalAlphaAttachment->image2DMultisample(numSamples, GL_R32F, size, GL_FALSE);
    m_depthAttachment->image2DMultisample(numSamples, GL_DEPTH_COMPONENT24, size, GL_FALSE);
}

void StochasticTransparency::updateNumSamples()
{
    setupMasksTexture();
    updateFramebuffer();
    updateNumSamplesUniforms();
}

void StochasticTransparency::updateNumSamplesUniforms()
{
    m_compositingProgram->setUniform("numSamples", static_cast<int>(m_options->numSamples()));
}

void StochasticTransparency::clearBuffers()
{
    m_fbo->setDrawBuffers({ kOpaqueColorAttachment, kTransparentColorAttachment, kTotalAlphaAttachment });
    
    m_fbo->clearBuffer(GL_COLOR, 0, glm::vec4{0.85f, 0.87f, 0.91f, 1.0f});
    m_fbo->clearBuffer(GL_COLOR, 1, glm::vec4{0.0f});
    m_fbo->clearBuffer(GL_COLOR, 2, glm::vec4{1.0f});
    m_fbo->clearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0.0f);
}

void StochasticTransparency::updateUniforms()
{
    const auto transform = m_projectionCapability->projection() * m_cameraCapability->view();
    const auto eye = m_cameraCapability->eye();
    const auto transparency = static_cast<unsigned int>(m_options->transparency());
    
    m_grid->update(eye, transform);
    
    auto updateProgramUniforms = [&transform, &transparency] (Program * program)
    {
        program->setUniform("transform", transform);
        program->setUniform("transparency", transparency);
    };
    
    updateProgramUniforms(m_totalAlphaProgram);
    updateProgramUniforms(m_alphaToCoverageProgram);
    updateProgramUniforms(m_colorAccumulationProgram);
}

void StochasticTransparency::renderOpaqueGeometry()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    m_fbo->bind(GL_FRAMEBUFFER);
    m_fbo->setDrawBuffer(kOpaqueColorAttachment);

    m_grid->draw();
}

void StochasticTransparency::renderTransparentGeometry()
{
    if (m_options->backFaceCulling())
        glEnable(GL_CULL_FACE);
    
    renderTotalAlpha();
    
    glEnable(GL_SAMPLE_SHADING);
    glMinSampleShading(1.0);

    if (m_options->optimization() == StochasticTransparencyOptimization::AlphaCorrection)
    {
        renderAlphaToCoverage();
    }
    else if (m_options->optimization() == StochasticTransparencyOptimization::AlphaCorrectionAndDepthBased)
    {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        renderAlphaToCoverage();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        
        renderColorAccumulation();
    }
    
    glDisable(GL_SAMPLE_SHADING);
    glDisable(GL_CULL_FACE);
}

void StochasticTransparency::renderTotalAlpha()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    glEnable (GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    
    m_fbo->bind(GL_FRAMEBUFFER);
    m_fbo->setDrawBuffer(kTotalAlphaAttachment);
    
    m_totalAlphaProgram->use();
    
    for (auto & drawable : m_drawables)
        drawable->draw();
    
    m_totalAlphaProgram->release();
    
    glDisable(GL_BLEND);
}

void StochasticTransparency::renderAlphaToCoverage()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    m_fbo->bind(GL_FRAMEBUFFER);
    m_fbo->setDrawBuffer(kTransparentColorAttachment);
    
    m_masksTexture->bindActive(GL_TEXTURE0);

    m_alphaToCoverageProgram->use();

    for (auto & drawable : m_drawables)
        drawable->draw();

    m_alphaToCoverageProgram->release();
}

void StochasticTransparency::renderColorAccumulation()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    glDepthFunc(GL_LEQUAL);
    
    glEnable (GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    
    m_fbo->bind(GL_FRAMEBUFFER);
    m_fbo->setDrawBuffer(kTransparentColorAttachment);
    
    m_colorAccumulationProgram->use();
    
    for (auto & drawable : m_drawables)
        drawable->draw();
    
    m_colorAccumulationProgram->release();

    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
}

void StochasticTransparency::composite()
{
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    
    auto targetfbo = m_targetFramebufferCapability->framebuffer();
    
    if (!targetfbo)
        targetfbo = Framebuffer::defaultFBO();
    
    targetfbo->bind(GL_FRAMEBUFFER);
    
    m_opaqueColorAttachment->bindActive(GL_TEXTURE0);
    m_totalAlphaAttachment->bindActive(GL_TEXTURE1);
    m_transparentColorAttachment->bindActive(GL_TEXTURE2);
    
    m_compositingQuad->draw();
    
    const auto rect = std::array<GLint, 4>{{
        m_viewportCapability->x(),
        m_viewportCapability->y(),
        m_viewportCapability->width(),
        m_viewportCapability->height()
    }};

    m_fbo->blit(GL_COLOR_ATTACHMENT0, rect, targetfbo, GL_BACK_LEFT, rect, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
