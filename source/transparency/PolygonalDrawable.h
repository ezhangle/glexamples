#pragma once

#include <glbinding/gl/types.h>

#include <globjects/base/ref_ptr.h>


namespace globjects
{
    class Buffer;
    class VertexArray;
}

class PolygonalGeometry;

class PolygonalDrawable
{
public:
    PolygonalDrawable(const PolygonalGeometry & geometry);

    void draw();

private:
    globjects::ref_ptr<globjects::VertexArray> m_vao;
    globjects::ref_ptr<globjects::Buffer> m_indices;
    globjects::ref_ptr<globjects::Buffer> m_vertices;
    globjects::ref_ptr<globjects::Buffer> m_normals;
    gl::GLsizei m_size;
};
