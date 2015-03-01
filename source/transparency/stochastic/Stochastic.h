#pragma once

#include <memory>
#include <vector>

#include <glbinding/gl/types.h>

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
    class TypedRenderTargetCapability;
    class AbstractVirtualTimeCapability;
}

class PolygonalDrawable;

class Stochastic : public gloperate::Painter
{
public:
    Stochastic(gloperate::ResourceManager & resourceManager);
    virtual ~Stochastic();
    
    virtual reflectionzeug::PropertyGroup * propertyGroup() const override;
    
public:
    void setupPropertyGroup();
    
    float transparency() const;
    void setTransparency(float transparency);
    
protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

    void onTargetFramebufferChanged();

protected:
    void setupFramebuffer();
    void setupProjection();
    void setupDrawable();
    void setupProgram();
    void setupMasksTexture();
    void updateFramebuffer();

protected:
    /* capabilities */
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::TypedRenderTargetCapability * m_typedRenderTargetCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;
    gloperate::AbstractVirtualTimeCapability * m_timeCapability;

    /* members */
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_colorAttachment;
    globjects::ref_ptr<globjects::Texture> m_depthAttachment;
    
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
    gl::GLint m_transparencyLocation;
    std::vector<std::unique_ptr<PolygonalDrawable>> m_drawables;
    
    globjects::ref_ptr<globjects::Texture> m_masksTexture;
    
    std::unique_ptr<reflectionzeug::PropertyGroup> m_propertyGroup;
    float m_transparency;
};
