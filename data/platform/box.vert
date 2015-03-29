#version 140
#extension GL_ARB_explicit_attrib_location : require

uniform mat4 transform;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_uv;

flat out vec3 v_normal;
smooth out vec2 v_uv;
out vec3 v_worldCoord;

void main()
{
    v_normal = normalize(a_normal);
    v_worldCoord = a_vertex;
    v_uv = vec2(a_uv.x, a_uv.y);
    gl_Position = transform * vec4(a_vertex, 1.0);
}
