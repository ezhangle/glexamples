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
    const gl::GLfloat half = (total/2)/total;
    const gl::GLfloat edgehalf = height+offset+height/2;

    const gl::GLfloat c0 = 0;
    const gl::GLfloat c1 = height/total;
    const gl::GLfloat c2 = (height+offset)/total;
    const gl::GLfloat c3 = (2*height+offset)/total;
    const gl::GLfloat c4 = (2*height+2*offset)/total;
    const gl::GLfloat c5 = (size+2*height)/total;
    const gl::GLfloat c6 = (size+2*height+offset)/total;

    auto mirror = [&] (const gl::GLfloat point, const gl::GLfloat axis, const gl::GLfloat direction) {
        return axis - direction*axis + direction*point;
    };

    auto face_z = [&] (const vec3 d) {

        auto texvec = [&] (const gl::GLfloat point1, const gl::GLfloat point2) {
            return vec3(point1, mirror(point2, half, d.z), 0);
        };

        return std::vector<vec3>{
            vec3( x, y, x*d.z), d, texvec(c6, c3)
        ,   vec3(-x, y, x*d.z), d, texvec(c3, c3)
        ,   vec3(-x,-y, x*d.z), d, texvec(c3, c2)
        ,   vec3(-x,-y, x*d.z), d, texvec(c3, c2)
        ,   vec3( x,-y, x*d.z), d, texvec(c6, c2)
        ,   vec3( x, y, x*d.z), d, texvec(c6, c3)

        ,   vec3( x-offset, y, (x-offset)*d.z ), -d, texvec(c5, c0)
        ,   vec3(-x+offset,-y, (x-offset)*d.z ), -d, texvec(c4, c1)
        ,   vec3(-x+offset, y, (x-offset)*d.z ), -d, texvec(c4, c0)
        ,   vec3( x-offset, y, (x-offset)*d.z ), -d, texvec(c5, c0)
        ,   vec3( x-offset,-y, (x-offset)*d.z ), -d, texvec(c5, c1)
        ,   vec3(-x+offset,-y, (x-offset)*d.z ), -d, texvec(c4, c1)
        };
    };

    auto face_x = [&] (const vec3 d) {

        auto texvec = [&] (const gl::GLfloat point1, const gl::GLfloat point2) {
            return vec3(mirror(point1, half, -d.x), point2, 0);
        };

        return std::vector<vec3>{
            vec3( x*d.x,-y, x), d, texvec(c2, c3)
        ,   vec3( x*d.x, y,-x), d, texvec(c3, c6)
        ,   vec3( x*d.x, y, x), d, texvec(c3, c3)
        ,   vec3( x*d.x,-y, x), d, texvec(c2, c3)
        ,   vec3( x*d.x,-y,-x), d, texvec(c2, c6)
        ,   vec3( x*d.x, y,-x), d, texvec(c3, c6)

        ,   vec3( (x-offset)*d.x, y, x-offset ), -d, texvec(c0, c4)
        ,   vec3( (x-offset)*d.x,-y, x-offset ), -d, texvec(c1, c4)
        ,   vec3( (x-offset)*d.x,-y,-x+offset ), -d, texvec(c1, c5)
        ,   vec3( (x-offset)*d.x, y, x-offset ), -d, texvec(c0, c4)
        ,   vec3( (x-offset)*d.x,-y,-x+offset ), -d, texvec(c1, c5)
        ,   vec3( (x-offset)*d.x, y,-x+offset ), -d, texvec(c0, c5)
        };
    };

    auto edge_z = [&] (const vec3 d, const vec3 n) {

        auto texvec = [&] (const gl::GLfloat point1, const gl::GLfloat point2) {
            return vec3(point1, mirror(mirror(point2, edgehalf, n.y), half, d.z), 0);
        };

        return std::vector<vec3>{
            vec3(-x       , n.y*y, d.z*x          ), n, texvec(c3, c3)
        ,   vec3(-x+offset, n.y*y, d.z*x          ), n, texvec(c4, c3)
        ,   vec3(-x+offset, n.y*y, d.z*(x-offset) ), n, texvec(c4, c4)
        ,   vec3( x-offset, n.y*y, d.z*x          ), n, texvec(c5, c3)
        ,   vec3( x       , n.y*y, d.z*x          ), n, texvec(c6, c3)
        ,   vec3( x-offset, n.y*y, d.z*(x-offset) ), n, texvec(c5, c4)

        ,   vec3(-x+offset, n.y*y, d.z*x          ), n, texvec(c4, c3)
        ,   vec3( x-offset, n.y*y, d.z*x          ), n, texvec(c5, c3)
        ,   vec3( x-offset, n.y*y, d.z*(x-offset) ), n, texvec(c5, c4)
        ,   vec3(-x+offset, n.y*y, d.z*x          ), n, texvec(c4, c3)
        ,   vec3( x-offset, n.y*y, d.z*(x-offset) ), n, texvec(c5, c4)
        ,   vec3(-x+offset, n.y*y, d.z*(x-offset) ), n, texvec(c4, c4)
        };
    };

    auto edge_x = [&] (const vec3 d, const vec3 n) {

        auto texvec = [&] (const gl::GLfloat point1, const gl::GLfloat point2) {
            return vec3(mirror(mirror(point1, edgehalf, n.y), half, -d.x), point2, 0);
        };

        return std::vector<vec3>{
            vec3( d.x*x         , n.y*y, x        ), n, texvec(c3, c3)
        ,   vec3( d.x*x         , n.y*y, x-offset ), n, texvec(c3, c4)
        ,   vec3( d.x*(x-offset), n.y*y, x-offset ), n, texvec(c4, c4)
        ,   vec3( d.x*x         , n.y*y,-x+offset ), n, texvec(c3, c5)
        ,   vec3( d.x*x         , n.y*y,-x        ), n, texvec(c3, c6)
        ,   vec3( d.x*(x-offset), n.y*y,-x+offset ), n, texvec(c4, c5)

        ,   vec3( d.x*(x-offset), n.y*y, x-offset ), n, texvec(c4, c4)
        ,   vec3( d.x*x         , n.y*y, x-offset ), n, texvec(c3, c4)
        ,   vec3( d.x*x         , n.y*y,-x+offset ), n, texvec(c3, c5)
        ,   vec3( d.x*(x-offset), n.y*y, x-offset ), n, texvec(c4, c4)
        ,   vec3( d.x*x         , n.y*y,-x+offset ), n, texvec(c3, c5)
        ,   vec3( d.x*(x-offset), n.y*y,-x+offset ), n, texvec(c4, c5)
        };
    };

    auto plate = [&] (const vec3 n) {
        return std::vector<vec3>{
            vec3(-x+offset, y-(offset/2), x-offset), n, vec3(c4, c4, 0)
        ,   vec3( x-offset, y-(offset/2), x-offset), n, vec3(c5, c4, 0)
        ,   vec3( x-offset, y-(offset/2),-x+offset), n, vec3(c5, c5, 0)
        ,   vec3(-x+offset, y-(offset/2), x-offset), n, vec3(c4, c4, 0)
        ,   vec3( x-offset, y-(offset/2),-x+offset), n, vec3(c5, c5, 0)
        ,   vec3(-x+offset, y-(offset/2),-x+offset), n, vec3(c4, c5, 0)
        };
    };

    std::vector<vec3> ret = face_z(vec3(0,0,1));
    std::vector<vec3> zminus = face_z(vec3(0,0,-1));
    std::vector<vec3> xplus = face_x(vec3(1,0,0));
    std::vector<vec3> xminus = face_x(vec3(-1,0,0));
    std::vector<vec3> edgezplustop = edge_z(vec3(0,0,1), vec3(0,1,0));
    std::vector<vec3> edgezplusbot = edge_z(vec3(0,0,1), vec3(0,-1,0));
    std::vector<vec3> edgezminustop = edge_z(vec3(0,0,-1), vec3(0,1,0));
    std::vector<vec3> edgezminusbot = edge_z(vec3(0,0,-1), vec3(0,-1,0));
    std::vector<vec3> edgexplustop = edge_x(vec3(1,0,0), vec3(0,1,0));
    std::vector<vec3> edgexplusbot = edge_x(vec3(1,0,0), vec3(0,-1,0));
    std::vector<vec3> edgexminustop = edge_x(vec3(-1,0,0), vec3(0,1,0));
    std::vector<vec3> edgexminusbot = edge_x(vec3(-1,0,0), vec3(0,-1,0));
    std::vector<vec3> middleplate = plate(vec3(0,1,0));

    ret.insert(ret.end(), zminus.begin(), zminus.end());
    ret.insert(ret.end(), xplus.begin(), xplus.end());
    ret.insert(ret.end(), xminus.begin(), xminus.end());
    ret.insert(ret.end(), edgezplustop.begin(), edgezplustop.end());
    ret.insert(ret.end(), edgezplusbot.begin(), edgezplusbot.end());
    ret.insert(ret.end(), edgezminustop.begin(), edgezminustop.end());
    ret.insert(ret.end(), edgezminusbot.begin(), edgezminusbot.end());
    ret.insert(ret.end(), edgexplustop.begin(), edgexplustop.end());
    ret.insert(ret.end(), edgexplusbot.begin(), edgexplusbot.end());
    ret.insert(ret.end(), edgexminustop.begin(), edgexminustop.end());
    ret.insert(ret.end(), edgexminusbot.begin(), edgexminusbot.end());
    ret.insert(ret.end(), middleplate.begin(), middleplate.end());

    return ret;
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

