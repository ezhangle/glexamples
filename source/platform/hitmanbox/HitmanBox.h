#pragma once

#include <array>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <globjects/base/Referenced.h>
#include <globjects/base/ref_ptr.h>

#include <globjects/VertexArray.h>
#include <globjects/Buffer.h>

#include <globjects/globjects_api.h>



class GLOBJECTS_API HitmanBox : public globjects::Referenced
{
public:
    using Face = std::array<gl::GLushort, 3>;

    static std::vector<glm::vec3> vertices(
        const gl::GLfloat size
    ,   const gl::GLfloat height
    ,   const gl::GLfloat offset);

public:
    HitmanBox(
        const gl::GLfloat size = 2.0f
    ,   const gl::GLfloat height = 0.25f
    ,   const gl::GLfloat offset = 0.2f
    ,   const gl::GLint positionLocation = 0
    ,   const gl::GLint normalLocation = 1
    ,   const gl::GLint textureLocation = 2);


    void draw();
    void draw(gl::GLenum mode);


private:
    globjects::ref_ptr<globjects::VertexArray> m_vao;

    globjects::ref_ptr<globjects::Buffer> m_vertices;

    gl::GLsizei m_size;

protected:
    virtual ~HitmanBox();
};

