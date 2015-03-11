#version 150 core
#extension GL_ARB_sample_shading : require
#extension GL_ARB_explicit_attrib_location : require

in vec3 v_normal;
flat in float v_rand;

layout(location = 0) out vec4 fragColor;

uniform uint transparency;
uniform sampler2D masksTexture;
uniform vec2 viewport;


float rand();
float calculateAlpha(uint mask);

const float denormFactor = pow(2.0, 8.0) - 1.0;

void main()
{
    ivec2 index = ivec2(rand() * 1023.0, transparency);
    uint mask = uint(texelFetch(masksTexture, index, 0).r * denormFactor);

    uint sampleBit = 1u << gl_SampleID;
    if ((mask & sampleBit) != sampleBit)
        discard;

    vec3 color = vec3(v_normal * 0.5 + 0.5);
    fragColor = vec4(color, 1.0);
}

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float rand()
{
    vec2 normFragCoord = floor(gl_FragCoord.xy) / viewport * v_rand;
    return rand(normFragCoord.xy);
}
