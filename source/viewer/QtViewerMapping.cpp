#include "QtViewerMapping.h"

#include <gloperate/painter/Camera.h>
#include <gloperate/painter/AbstractCameraCapability.h>
#include <gloperate/painter/AbstractProjectionCapability.h>
#include <gloperate/painter/AbstractViewportCapability.h>
#include <gloperate/painter/AbstractTypedRenderTargetCapability.h>
#include <gloperate/painter/Painter.h>
#include <gloperate/input/AbstractEvent.h>
#include <gloperate/input/KeyboardEvent.h>
#include <gloperate/input/MouseEvent.h>
#include <gloperate/input/WheelEvent.h>
#include <gloperate/navigation/WorldInHandNavigation.h>
#include <gloperate/tools/CoordinateProvider.h>
#include <gloperate-qt/QtOpenGLWindow.h>

#include "util.hpp"


using namespace gloperate;
using namespace gloperate_qt;

QtViewerMapping::QtViewerMapping(QtOpenGLWindow * window)
:   AbstractQtMapping{window}
{
}

QtViewerMapping::~QtViewerMapping() = default;

void QtViewerMapping::initializeTools()
{
    if (m_painter &&
        m_painter->supports<AbstractCameraCapability>() &&
        m_painter->supports<AbstractViewportCapability>() &&
        m_painter->supports<AbstractTypedRenderTargetCapability>() &&
        m_painter->supports<AbstractProjectionCapability>())
    {
        auto cameraCapability = m_painter->getCapability<AbstractCameraCapability>();
        auto projectionCapability = m_painter->getCapability<AbstractProjectionCapability>();
        auto renderTargetCapability = m_painter->getCapability<AbstractTypedRenderTargetCapability>();
        auto viewportCapability = m_painter->getCapability<AbstractViewportCapability>();

        m_coordProvider = make_unique<CoordinateProvider>(
            cameraCapability,
            projectionCapability,
            viewportCapability,
            renderTargetCapability);

        m_navigation = make_unique<WorldInHandNavigation>(
            *cameraCapability,
            *viewportCapability,
            *m_coordProvider);
    }
}

void QtViewerMapping::mapEvent(AbstractEvent * event)
{
    switch (event->sourceType())
    {
        case SourceType::Keyboard:
            mapKeyboardEvent(event);
            break;
        case SourceType::Mouse:
            mapMouseEvent(event);
            break;
        case SourceType::Wheel:
            mapWheelEvent(event);
            break;
        default:
            break;
    }
}

void QtViewerMapping::mapKeyboardEvent(gloperate::AbstractEvent * event)
{
    const auto keyEvent = dynamic_cast<KeyboardEvent * >(event);
    
    if (!keyEvent)
        return;
    
    if (keyEvent->type() != KeyboardEvent::Type::Press)
        return;

    switch (keyEvent->key())
    {
            // WASD move camera
        case KeyW:
            m_navigation->pan(glm::vec3(0, 0, 1));
            break;
        case KeyA:
            m_navigation->pan(glm::vec3(1, 0, 0));
            break;
        case KeyS:
            m_navigation->pan(glm::vec3(0, 0, -1));
            break;
        case KeyD:
            m_navigation->pan(glm::vec3(-1, 0, 0));
            break;
            // Reset camera position
        case KeyR:
            m_navigation->reset();
            break;
            // Arrows rotate camera
        case KeyUp:
            m_navigation->rotate(0.0f, glm::radians(-10.0f));
            break;
        case KeyLeft:
            m_navigation->rotate(glm::radians(10.0f), 0.0f);
            break;
        case KeyDown:
            m_navigation->rotate(0.0f, glm::radians(10.0f));
            break;
        case KeyRight:
            m_navigation->rotate(glm::radians(-10.0f), 0.0f);
            break;
        default:
            break;
    }
}

void QtViewerMapping::mapMouseEvent(gloperate::AbstractEvent * event)
{
    const auto mouseEvent = dynamic_cast<MouseEvent *>(event);
    
    if (!mouseEvent)
        return;
    
    const auto mousePos = mouseEvent->pos() * static_cast<int>(m_window->devicePixelRatio());
    
    if (mouseEvent->type() == MouseEvent::Type::Press)
    {
        switch (mouseEvent->button())
        {
            case MouseButtonMiddle:
                m_navigation->reset();
                break;
            case MouseButtonLeft:
                m_navigation->panBegin(mousePos);
                break;
            case MouseButtonRight:
                m_navigation->rotateBegin(mousePos);
                break;
            default:
                break;
        }
    }
    else if (mouseEvent->type() == MouseEvent::Type::Move)
    {
        switch (m_navigation->mode())
        {
            case WorldInHandNavigation::InteractionMode::PanInteraction:
                m_navigation->panProcess(mousePos);
                break;
            case WorldInHandNavigation::InteractionMode::RotateInteraction:
                m_navigation->rotateProcess(mousePos);
                break;
            default:
                break;
        }
    }
    else if (mouseEvent->type() == MouseEvent::Type::Release)
    {
        switch (mouseEvent->button())
        {
            case MouseButtonLeft:
                m_navigation->panEnd();
                break;
            case MouseButtonRight:
                m_navigation->rotateEnd();
                break;
            default:
                break;
        }
    }
}

void QtViewerMapping::mapWheelEvent(gloperate::AbstractEvent * event)
{
    auto wheelEvent = dynamic_cast<WheelEvent *>(event);
    
    if (!wheelEvent)
        return;
    
    const auto mousePos = wheelEvent->pos() * static_cast<int>(m_window->devicePixelRatio());
    
    auto scale = wheelEvent->angleDelta().y;
    scale /= WheelEvent::defaultMouseAngleDelta();
    scale *= 0.1f; // smoother (slower) scaling
    m_navigation->scaleAtMouse(mousePos, scale);
}

