#include "HitmanBox.h"

#include <algorithm>

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>

#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Buffer.h>


using namespace glm;
using namespace globjects;



std::vector<vec3> HitmanBox::vertices(const gl::GLfloat size, const gl::GLfloat height, const gl::GLfloat offset)
{
    const gl::GLfloat x = size / 2;
    const gl::GLfloat y = height / 2;
    const gl::GLfloat total = size + 4*height + 2*offset;

    const gl::GLfloat c0 = 0;
    const gl::GLfloat c1 = height/total;
    const gl::GLfloat c2 = (height+offset)/total;
    const gl::GLfloat c3 = (2*height+offset)/total;
    const gl::GLfloat c4 = (2*height+2*offset)/total;
    const gl::GLfloat c5 = (size+2*height)/total;
    const gl::GLfloat c6 = (size+2*height+offset)/total;
    const gl::GLfloat c7 = (size+3*height+offset)/total;
    const gl::GLfloat c8 = (size+3*height+2*offset)/total;
    const gl::GLfloat c9 = (size+4*height+2*offset)/total;


    return {
        
    // plus Z (frontface)
        vec3( x, y, x), vec3(0,0,1), vec3(c6, c3, 0)
    ,   vec3(-x, y, x), vec3(0,0,1), vec3(c3, c3, 0)
    ,   vec3(-x,-y, x), vec3(0,0,1), vec3(c3, c2, 0)
    ,   vec3(-x,-y, x), vec3(0,0,1), vec3(c3, c2, 0)
    ,   vec3( x,-y, x), vec3(0,0,1), vec3(c6, c2, 0)
    ,   vec3( x, y, x), vec3(0,0,1), vec3(c6, c3, 0)

    // plus Z (backface)
    ,   vec3( x-offset, y, x-offset ), vec3(0,0,-1), vec3(c5, c0, 0)
    ,   vec3(-x+offset,-y, x-offset ), vec3(0,0,-1), vec3(c4, c1, 0)
    ,   vec3(-x+offset, y, x-offset ), vec3(0,0,-1), vec3(c4, c0, 0)
    ,   vec3( x-offset, y, x-offset ), vec3(0,0,-1), vec3(c5, c0, 0)
    ,   vec3( x-offset,-y, x-offset ), vec3(0,0,-1), vec3(c5, c1, 0)
    ,   vec3(-x+offset,-y, x-offset ), vec3(0,0,-1), vec3(c4, c1, 0)

    // plus Z (top)
    ,   vec3(-x       , y, x       ), vec3(0,1,0),  vec3(c3, c3, 0)
    ,   vec3(-x+offset, y, x       ), vec3(0,1,0),  vec3(c4, c3, 0)
    ,   vec3(-x+offset, y, x-offset), vec3(0,1,0),  vec3(c4, c4, 0)
    ,   vec3( x-offset, y, x       ), vec3(0,1,0),  vec3(c5, c3, 0)
    ,   vec3( x       , y, x       ), vec3(0,1,0),  vec3(c6, c3, 0)
    ,   vec3( x-offset, y, x-offset), vec3(0,1,0),  vec3(c5, c4, 0)
    ,   vec3(-x+offset, y, x       ), vec3(0,1,0),  vec3(c4, c3, 0)
    ,   vec3( x-offset, y, x       ), vec3(0,1,0),  vec3(c5, c3, 0)
    ,   vec3( x-offset, y, x-offset), vec3(0,1,0),  vec3(c5, c4, 0)
    ,   vec3(-x+offset, y, x       ), vec3(0,1,0),  vec3(c4, c3, 0)
    ,   vec3( x-offset, y, x-offset), vec3(0,1,0),  vec3(c5, c4, 0)
    ,   vec3(-x+offset, y, x-offset), vec3(0,1,0),  vec3(c4, c4, 0)

    // plus Z (bottom)
    ,   vec3(-x       ,-y, x       ), vec3(0,-1,0), vec3(c3, c2, 0)
    ,   vec3(-x+offset,-y, x-offset), vec3(0,-1,0), vec3(c4, c1, 0)
    ,   vec3(-x+offset,-y, x       ), vec3(0,-1,0), vec3(c4, c2, 0)
    ,   vec3( x-offset,-y, x       ), vec3(0,-1,0), vec3(c5, c2, 0)
    ,   vec3( x-offset,-y, x-offset), vec3(0,-1,0), vec3(c5, c1, 0)
    ,   vec3( x       ,-y, x       ), vec3(0,-1,0), vec3(c6, c2, 0)
    ,   vec3(-x+offset,-y, x       ), vec3(0,-1,0), vec3(c4, c2, 0)
    ,   vec3( x-offset,-y, x-offset), vec3(0,-1,0), vec3(c5, c1, 0)
    ,   vec3( x-offset,-y, x       ), vec3(0,-1,0), vec3(c5, c2, 0)
    ,   vec3(-x+offset,-y, x       ), vec3(0,-1,0), vec3(c4, c2, 0)
    ,   vec3(-x+offset,-y, x-offset), vec3(0,-1,0), vec3(c4, c1, 0)
    ,   vec3( x-offset,-y, x-offset), vec3(0,-1,0), vec3(c5, c1, 0)

    // minus Z (frontface)
    ,   vec3( x, y, -x), vec3(0,0,-1), vec3(c6, c6, 0)
    ,   vec3(-x,-y, -x), vec3(0,0,-1), vec3(c3, c7, 0)
    ,   vec3(-x, y, -x), vec3(0,0,-1), vec3(c3, c6, 0)
    ,   vec3(-x,-y, -x), vec3(0,0,-1), vec3(c3, c7, 0)
    ,   vec3( x, y, -x), vec3(0,0,-1), vec3(c6, c6, 0)
    ,   vec3( x,-y, -x), vec3(0,0,-1), vec3(c6, c7, 0)

    // minus Z (backface)
    ,   vec3( x-offset, y, -x+offset ), vec3(0,0,1), vec3(c5, c9, 0)
    ,   vec3(-x+offset, y, -x+offset ), vec3(0,0,1), vec3(c4, c9, 0)
    ,   vec3(-x+offset,-y, -x+offset ), vec3(0,0,1), vec3(c4, c8, 0)
    ,   vec3( x-offset, y, -x+offset ), vec3(0,0,1), vec3(c5, c9, 0)
    ,   vec3(-x+offset,-y, -x+offset ), vec3(0,0,1), vec3(c4, c8, 0)
    ,   vec3( x-offset,-y, -x+offset ), vec3(0,0,1), vec3(c5, c8, 0)

    // minus Z (top)
    ,   vec3(-x       , y, -x       ), vec3(0,1,0),  vec3(c3, c6, 0)
    ,   vec3(-x+offset, y, -x+offset), vec3(0,1,0),  vec3(c4, c5, 0)
    ,   vec3(-x+offset, y, -x       ), vec3(0,1,0),  vec3(c4, c6, 0)
    ,   vec3( x-offset, y, -x       ), vec3(0,1,0),  vec3(c5, c6, 0)
    ,   vec3( x-offset, y, -x+offset), vec3(0,1,0),  vec3(c5, c5, 0)
    ,   vec3( x       , y, -x       ), vec3(0,1,0),  vec3(c6, c6, 0)
    ,   vec3(-x+offset, y, -x       ), vec3(0,1,0),  vec3(c4, c6, 0)
    ,   vec3( x-offset, y, -x+offset), vec3(0,1,0),  vec3(c5, c5, 0)
    ,   vec3( x-offset, y, -x       ), vec3(0,1,0),  vec3(c5, c6, 0)
    ,   vec3(-x+offset, y, -x       ), vec3(0,1,0),  vec3(c4, c6, 0)
    ,   vec3(-x+offset, y, -x+offset), vec3(0,1,0),  vec3(c4, c5, 0)
    ,   vec3( x-offset, y, -x+offset), vec3(0,1,0),  vec3(c5, c5, 0)

    // minus Z (bottom)
    ,   vec3(-x       ,-y, -x       ), vec3(0,-1,0), vec3(c3, c7, 0)
    ,   vec3(-x+offset,-y, -x       ), vec3(0,-1,0), vec3(c4, c7, 0)
    ,   vec3(-x+offset,-y, -x+offset), vec3(0,-1,0), vec3(c4, c8, 0)
    ,   vec3( x-offset,-y, -x       ), vec3(0,-1,0), vec3(c5, c7, 0)
    ,   vec3( x       ,-y, -x       ), vec3(0,-1,0), vec3(c6, c7, 0)
    ,   vec3( x-offset,-y, -x+offset), vec3(0,-1,0), vec3(c5, c8, 0)
    ,   vec3(-x+offset,-y, -x       ), vec3(0,-1,0), vec3(c4, c7, 0)
    ,   vec3( x-offset,-y, -x       ), vec3(0,-1,0), vec3(c5, c7, 0)
    ,   vec3( x-offset,-y, -x+offset), vec3(0,-1,0), vec3(c5, c8, 0)
    ,   vec3(-x+offset,-y, -x       ), vec3(0,-1,0), vec3(c4, c7, 0)
    ,   vec3( x-offset,-y, -x+offset), vec3(0,-1,0), vec3(c5, c8, 0)
    ,   vec3(-x+offset,-y, -x+offset), vec3(0,-1,0), vec3(c4, c8, 0)

    // plus X (frontface)
    ,   vec3( x,-y, x), vec3(1,0,0), vec3(c7, c3, 0)
    ,   vec3( x, y,-x), vec3(1,0,0), vec3(c6, c6, 0)
    ,   vec3( x, y, x), vec3(1,0,0), vec3(c6, c3, 0)
    ,   vec3( x,-y, x), vec3(1,0,0), vec3(c7, c3, 0)
    ,   vec3( x,-y,-x), vec3(1,0,0), vec3(c7, c6, 0)
    ,   vec3( x, y,-x), vec3(1,0,0), vec3(c6, c6, 0)

    // plus X (backface)
    ,   vec3( x-offset, y, x-offset ), vec3(-1,0,0), vec3(c9, c4, 0)
    ,   vec3( x-offset,-y,-x+offset ), vec3(-1,0,0), vec3(c8, c5, 0)
    ,   vec3( x-offset,-y, x-offset ), vec3(-1,0,0), vec3(c8, c4, 0)
    ,   vec3( x-offset, y, x-offset ), vec3(-1,0,0), vec3(c9, c4, 0)
    ,   vec3( x-offset, y,-x+offset ), vec3(-1,0,0), vec3(c9, c5, 0)
    ,   vec3( x-offset,-y,-x+offset ), vec3(-1,0,0), vec3(c8, c5, 0)

    // plus X (top)
    ,   vec3( x         , y, x        ), vec3(0,1,0), vec3(c6, c3, 0)
    ,   vec3( x         , y, x-offset ), vec3(0,1,0), vec3(c6, c4, 0)
    ,   vec3( x-offset  , y, x-offset ), vec3(0,1,0), vec3(c5, c4, 0)
    ,   vec3( x         , y,-x+offset ), vec3(0,1,0), vec3(c6, c5, 0)
    ,   vec3( x         , y,-x        ), vec3(0,1,0), vec3(c6, c6, 0)
    ,   vec3( x-offset  , y,-x+offset ), vec3(0,1,0), vec3(c5, c5, 0)
    ,   vec3( x-offset  , y, x-offset ), vec3(0,1,0), vec3(c5, c4, 0)
    ,   vec3( x         , y, x-offset ), vec3(0,1,0), vec3(c6, c4, 0)
    ,   vec3( x         , y,-x+offset ), vec3(0,1,0), vec3(c6, c5, 0)
    ,   vec3( x-offset  , y, x-offset ), vec3(0,1,0), vec3(c5, c4, 0)
    ,   vec3( x         , y,-x+offset ), vec3(0,1,0), vec3(c6, c5, 0)
    ,   vec3( x-offset  , y,-x+offset ), vec3(0,1,0), vec3(c5, c5, 0)

    // plus X (bottom)
    ,   vec3( x         , -y, x        ), vec3(0,-1,0), vec3(c7, c3, 0)
    ,   vec3( x-offset  , -y, x-offset ), vec3(0,-1,0), vec3(c8, c4, 0)
    ,   vec3( x         , -y, x-offset ), vec3(0,-1,0), vec3(c7, c4, 0)
    ,   vec3( x         , -y,-x+offset ), vec3(0,-1,0), vec3(c7, c5, 0)
    ,   vec3( x-offset  , -y,-x+offset ), vec3(0,-1,0), vec3(c8, c5, 0)
    ,   vec3( x         , -y,-x        ), vec3(0,-1,0), vec3(c7, c6, 0)
    ,   vec3( x-offset  , -y, x-offset ), vec3(0,-1,0), vec3(c8, c4, 0)
    ,   vec3( x         , -y,-x+offset ), vec3(0,-1,0), vec3(c7, c5, 0)
    ,   vec3( x         , -y, x-offset ), vec3(0,-1,0), vec3(c7, c4, 0)
    ,   vec3( x-offset  , -y, x-offset ), vec3(0,-1,0), vec3(c8, c4, 0)
    ,   vec3( x-offset  , -y,-x+offset ), vec3(0,-1,0), vec3(c8, c5, 0)
    ,   vec3( x         , -y,-x+offset ), vec3(0,-1,0), vec3(c7, c5, 0)

    // minus X (frontface)
    ,   vec3(-x,-y, x), vec3(-1,0,0), vec3(c2, c3, 0)
    ,   vec3(-x, y, x), vec3(-1,0,0), vec3(c3, c3, 0)
    ,   vec3(-x, y,-x), vec3(-1,0,0), vec3(c3, c6, 0)
    ,   vec3(-x,-y, x), vec3(-1,0,0), vec3(c2, c3, 0)
    ,   vec3(-x, y,-x), vec3(-1,0,0), vec3(c3, c6, 0)
    ,   vec3(-x,-y,-x), vec3(-1,0,0), vec3(c2, c6, 0)

    // minus X (backface)
    ,   vec3(-x+offset, y, x-offset ), vec3(1,0,0), vec3(c0, c4, 0)
    ,   vec3(-x+offset,-y, x-offset ), vec3(1,0,0), vec3(c1, c4, 0)
    ,   vec3(-x+offset,-y,-x+offset ), vec3(1,0,0), vec3(c1, c5, 0)
    ,   vec3(-x+offset, y, x-offset ), vec3(1,0,0), vec3(c0, c4, 0)
    ,   vec3(-x+offset,-y,-x+offset ), vec3(1,0,0), vec3(c1, c5, 0)
    ,   vec3(-x+offset, y,-x+offset ), vec3(1,0,0), vec3(c0, c5, 0)

    // minus X (top)
    ,   vec3(-x         , y, x        ), vec3(0,1,0), vec3(c3, c3, 0)
    ,   vec3(-x+offset  , y, x-offset ), vec3(0,1,0), vec3(c4, c4, 0)
    ,   vec3(-x         , y, x-offset ), vec3(0,1,0), vec3(c3, c4, 0)
    ,   vec3(-x         , y,-x+offset ), vec3(0,1,0), vec3(c3, c5, 0)
    ,   vec3(-x+offset  , y,-x+offset ), vec3(0,1,0), vec3(c4, c5, 0)
    ,   vec3(-x         , y,-x        ), vec3(0,1,0), vec3(c3, c6, 0)
    ,   vec3(-x+offset  , y, x-offset ), vec3(0,1,0), vec3(c4, c4, 0)
    ,   vec3(-x         , y,-x+offset ), vec3(0,1,0), vec3(c3, c5, 0)
    ,   vec3(-x         , y, x-offset ), vec3(0,1,0), vec3(c3, c4, 0)
    ,   vec3(-x+offset  , y, x-offset ), vec3(0,1,0), vec3(c4, c4, 0)
    ,   vec3(-x+offset  , y,-x+offset ), vec3(0,1,0), vec3(c4, c5, 0)
    ,   vec3(-x         , y,-x+offset ), vec3(0,1,0), vec3(c3, c5, 0)

    // minus X (bottom)
    ,   vec3(-x         , -y, x        ), vec3(0,-1,0), vec3(c2, c3, 0)
    ,   vec3(-x         , -y, x-offset ), vec3(0,-1,0), vec3(c2, c4, 0)
    ,   vec3(-x+offset  , -y, x-offset ), vec3(0,-1,0), vec3(c1, c4, 0)
    ,   vec3(-x         , -y,-x+offset ), vec3(0,-1,0), vec3(c2, c5, 0)
    ,   vec3(-x         , -y,-x        ), vec3(0,-1,0), vec3(c2, c6, 0)
    ,   vec3(-x+offset  , -y,-x+offset ), vec3(0,-1,0), vec3(c1, c5, 0)
    ,   vec3(-x+offset  , -y, x-offset ), vec3(0,-1,0), vec3(c1, c4, 0)
    ,   vec3(-x         , -y, x-offset ), vec3(0,-1,0), vec3(c2, c4, 0)
    ,   vec3(-x         , -y,-x+offset ), vec3(0,-1,0), vec3(c2, c5, 0)
    ,   vec3(-x+offset  , -y, x-offset ), vec3(0,-1,0), vec3(c1, c4, 0)
    ,   vec3(-x         , -y,-x+offset ), vec3(0,-1,0), vec3(c2, c5, 0)
    ,   vec3(-x+offset  , -y,-x+offset ), vec3(0,-1,0), vec3(c1, c5, 0)

    // plate
    ,   vec3(-x+offset, y-(offset/2), x-offset), vec3(0,1,0), vec3(c4, c4, 0)
    ,   vec3( x-offset, y-(offset/2), x-offset), vec3(0,1,0), vec3(c5, c4, 0)
    ,   vec3( x-offset, y-(offset/2),-x+offset), vec3(0,1,0), vec3(c5, c5, 0)
    ,   vec3(-x+offset, y-(offset/2), x-offset), vec3(0,1,0), vec3(c4, c4, 0)
    ,   vec3( x-offset, y-(offset/2),-x+offset), vec3(0,1,0), vec3(c5, c5, 0)
    ,   vec3(-x+offset, y-(offset/2),-x+offset), vec3(0,1,0), vec3(c4, c5, 0)
    };
}


HitmanBox::HitmanBox(const gl::GLfloat size, const gl::GLfloat height, const gl::GLfloat offset, const gl::GLint positionLocation, const gl::GLint normalLocation, const gl::GLint textureLocation)
: m_vao(new VertexArray)
, m_vertices(new Buffer)
{
    auto v(vertices(size, height, offset));
    m_vertices->setData(v, gl::GL_STATIC_DRAW);

    m_size = static_cast<gl::GLsizei>(v.size() / 2);

    m_vao->bind();

    if (positionLocation >= 0)
    {
        auto vertexBinding = m_vao->binding(0);
        vertexBinding->setAttribute(positionLocation);
        vertexBinding->setBuffer(m_vertices, 0, static_cast<gl::GLint>(sizeof(vec3) * 3));
        vertexBinding->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE);
        m_vao->enable(0);
    }

    if (normalLocation >= 0)
    {
        auto normalBinding = m_vao->binding(1);
        normalBinding->setAttribute(normalLocation);
        normalBinding->setBuffer(m_vertices, 0, static_cast<gl::GLint>(sizeof(vec3) * 3));
        normalBinding->setFormat(3, gl::GL_FLOAT, gl::GL_TRUE, sizeof(vec3));
        m_vao->enable(1);
    }

    if (textureLocation >= 0)
    {
        auto textureBinding = m_vao->binding(2);
        textureBinding->setAttribute(textureLocation);
        textureBinding->setBuffer(m_vertices, 0, static_cast<gl::GLint>(sizeof(vec3) * 3));
        textureBinding->setFormat(3, gl::GL_FLOAT, gl::GL_FALSE, static_cast<gl::GLint>(sizeof(vec3) * 2));
        m_vao->enable(2);
    }

    m_vao->unbind();
}

HitmanBox::~HitmanBox()
{
}

void HitmanBox::draw()
{
    draw(gl::GL_TRIANGLES);
}

void HitmanBox::draw(const gl::GLenum mode)
{
    gl::glEnable(gl::GL_DEPTH_TEST);

    m_vao->bind();
    m_vao->drawArrays(mode, 0, m_size);
    m_vao->unbind();

    gl::glDisable(gl::GL_DEPTH_TEST);
}

