#version 150 core
#extension GL_ARB_sample_shading : require
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_normal;

layout(location = 0) out vec4 fragColor;

uniform uint transparency;


void main()
{
    float alpha = float(transparency) / 255.0;
    vec3 color = vec3(v_normal * 0.5 + 0.5);
    fragColor = vec4(color * alpha, alpha);
}
