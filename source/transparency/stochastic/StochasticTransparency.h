#pragma once

#include <memory>
#include <vector>

#include <glbinding/gl/types.h>
#include <glbinding/gl/enum.h>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>


namespace globjects
{
    class Framebuffer;
    class Program;
    class Texture;
}

namespace gloperate
{
    class AdaptiveGrid;
    class ResourceManager;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
    class ScreenAlignedQuad;
}

class PolygonalDrawable;
class StochasticTransparencyOptions;

class StochasticTransparency : public gloperate::Painter
{
public:
    StochasticTransparency(gloperate::ResourceManager & resourceManager);
    virtual ~StochasticTransparency() override;
    
protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

protected:
    void setupFramebuffer();
    void setupProjection();
    void setupPrograms();
    void setupMasksTexture();
    void setupDrawable();
    void updateFramebuffer();
    void updateNumSamples();
    void updateNumSamplesUniforms();
    
protected:
    void clearBuffers();
    void updateUniforms();
    void renderOpaqueGeometry();
    void renderTransparentGeometry();
    void renderTotalAlpha();
    void renderAlphaToCoverage(gl::GLenum colorAttachment);
    void renderColorAccumulation();
    void blit();
    void composite();

private:
    /** \name Capabilities */
    /** \{ */
    
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;
    
    /** \} */

    /** \name Framebuffers and Textures */
    /** \{ */
    
    static const auto kOpaqueColorAttachment = gl::GL_COLOR_ATTACHMENT0;
    static const auto kTransparentColorAttachment = gl::GL_COLOR_ATTACHMENT1;
    static const auto kTotalAlphaAttachment = gl::GL_COLOR_ATTACHMENT2;
    
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_opaqueColorAttachment;
    globjects::ref_ptr<globjects::Texture> m_transparentColorAttachment;
    globjects::ref_ptr<globjects::Texture> m_totalAlphaAttachment;
    globjects::ref_ptr<globjects::Texture> m_depthAttachment;
    
    /** \} */
    
    /** \name Programs */
    /** \{ */
    
    globjects::ref_ptr<globjects::Program> m_totalAlphaProgram;
    
    globjects::ref_ptr<globjects::Program> m_alphaToCoverageProgram;
    globjects::ref_ptr<globjects::Texture> m_masksTexture;
    
    globjects::ref_ptr<globjects::Program> m_colorAccumulationProgram;
    
    globjects::ref_ptr<globjects::Program> m_compositingProgram;
    
    /** \} */
    
    /** \name Geometry */
    /** \{ */
    
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    std::vector<std::unique_ptr<PolygonalDrawable>> m_drawables;
    globjects::ref_ptr<gloperate::ScreenAlignedQuad> m_compositingQuad;
    
    /** \} */

    /** \name Properties */
    /** \{ */
    
    std::unique_ptr<StochasticTransparencyOptions> m_options;
    
    /** \} */
};
