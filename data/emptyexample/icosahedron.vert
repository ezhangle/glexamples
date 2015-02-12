#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 a_vertex;
layout(location = 1) in vec3 a_normal;

out vec3 v_normal;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(a_vertex, 1.0);
    v_normal = a_normal;
}
