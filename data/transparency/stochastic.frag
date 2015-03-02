#version 150 core
#extension GL_ARB_sample_shading : enable

in vec3 v_normal;

out vec4 fragColor;

uniform uint transparency;
uniform vec2 viewport;
uniform sampler2D masks;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

void main()
{
    vec2 normFragCoord = gl_FragCoord.xy / viewport;
    ivec2 index = ivec2(rand(vec2(normFragCoord.x, gl_FragCoord.z)) * 1023, transparency);
    uint mask = uint(texelFetch(masks, index, 0).r * 255.0);

    uint sampleBit = uint(1) << gl_SampleID;
    if ((mask & sampleBit) != sampleBit)
        discard;

    fragColor = vec4(vec3(v_normal * 0.5 + 0.5), 1.0);
    // fragColor = vec4(vec3(float(mask) / 255.0 / 8.0), 1.0);
}
