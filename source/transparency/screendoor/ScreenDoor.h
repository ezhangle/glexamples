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
}

class PolygonalDrawable;

class ScreenDoor : public gloperate::Painter
{
public:
    ScreenDoor(gloperate::ResourceManager & resourceManager);
    virtual ~ScreenDoor();
    
public:
    void setupPropertyGroup();
    
    bool multisampling() const;
    void setMultisampling(bool b);
    
    float transparency() const;
    void setTransparency(float transparency);
    
protected:
    virtual void onInitialize() override;
    virtual void onPaint() override;

protected:
    void setupFramebuffer();
    void setupProjection();
    void setupDrawable();
    void setupProgram();
    void updateFramebuffer();

protected:
    /* capabilities */
    gloperate::AbstractTargetFramebufferCapability * m_targetFramebufferCapability;
    gloperate::AbstractViewportCapability * m_viewportCapability;
    gloperate::AbstractPerspectiveProjectionCapability * m_projectionCapability;
    gloperate::AbstractCameraCapability * m_cameraCapability;

    /* members */
    globjects::ref_ptr<globjects::Framebuffer> m_fbo;
    globjects::ref_ptr<globjects::Texture> m_colorAttachment;
    globjects::ref_ptr<globjects::Texture> m_depthAttachment;
    
    globjects::ref_ptr<gloperate::AdaptiveGrid> m_grid;
    globjects::ref_ptr<globjects::Program> m_program;
    gl::GLint m_transformLocation;
    gl::GLint m_transparencyLocation;
    std::vector<std::unique_ptr<PolygonalDrawable>> m_drawables;

    bool m_multisampling;
    bool m_multisamplingChanged;
    float m_transparency;
};
