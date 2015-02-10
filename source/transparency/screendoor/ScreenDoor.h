#pragma once

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
    class Icosahedron;
    class ResourceManager;
    class AbstractTargetFramebufferCapability;
    class AbstractViewportCapability;
    class AbstractPerspectiveProjectionCapability;
    class AbstractCameraCapability;
    class TypedRenderTargetCapability;
    class AbstractVirtualTimeCapability;
}

class ScreenDoor : public gloperate::Painter
{
public:
    ScreenDoor(gloperate::ResourceManager & resourceManager);
    virtual ~ScreenDoor();

    void setupProjection();

protected:
    virtual void onInitialize();
    virtual void onPaint();
    virtual void onTargetFramebufferChanged();

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
    globjects::ref_ptr<gloperate::Icosahedron> m_icosahedron;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
};
