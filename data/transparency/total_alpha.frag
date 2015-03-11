#version 150 core
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) out float fragTransparency;

uniform uint transparency;


void main()
{
    fragTransparency = float(transparency) / 255.0;
}
