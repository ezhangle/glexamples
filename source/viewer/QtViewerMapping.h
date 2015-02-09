#pragma once

#include <memory>

#include <gloperate-qt/AbstractQtMapping.h>


namespace gloperate
{
    class CoordinateProvider;
    class WorldInHandNavigation;
}

class QtViewerMapping : public gloperate_qt::AbstractQtMapping
{
public:
    QtViewerMapping(gloperate_qt::QtOpenGLWindow * window);
    virtual ~QtViewerMapping();

    virtual void initializeTools() override;

protected:
    virtual void mapEvent(gloperate::AbstractEvent * event) override;

private:
    void mapKeyboardEvent(gloperate::AbstractEvent * event);
    void mapMouseEvent(gloperate::AbstractEvent * event);
    void mapWheelEvent(gloperate::AbstractEvent * event);

protected:
    std::unique_ptr<gloperate::WorldInHandNavigation> m_navigation;
    std::unique_ptr<gloperate::CoordinateProvider> m_coordProvider;
};
