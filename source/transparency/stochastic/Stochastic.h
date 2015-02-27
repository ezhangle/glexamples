#pragma once

#include <memory>
#include <vector>

#include <glbinding/gl/types.h>

#include <globjects/base/ref_ptr.h>

#include <gloperate/painter/Painter.h>


namespace globjects
{
    class Program;
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

protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

    void onTargetFramebufferChanged();
    
    virtual reflectionzeug::PropertyGroup * propertyGroup() const override;

protected:
    void setupProjection();
    void setupDrawable();

protected:
    /* capabilities */
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::TypedRenderTargetCapability * m_typedRenderTargetCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;
    gloperate::AbstractVirtualTimeCapability * m_timeCapability;

    /* members */
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
    std::vector<std::unique_ptr<PolygonalDrawable>> m_drawables;
};
