#version 150 core
#extension GL_ARB_explicit_attrib_location : require

in vec2 v_uv;

layout (location = 0) out vec3 fragColor;

uniform sampler2DMS opaqueColorTexture;
uniform sampler2DMS totalAlphaTexture;
uniform sampler2DMS transparentColorTexture;
uniform int numSamples;


vec4 filteredTexelFetch(in sampler2DMS texture, in ivec2 coordinate)
{
    vec4 texelSum = vec4(0.0);

    for (int i = 0; i < numSamples; ++i)
        texelSum += texelFetch(texture, coordinate, i);

    return texelSum / float(numSamples);
}

void main()
{
    ivec2 coordinate = ivec2(gl_FragCoord.xy);

    vec3 opaqueColor = filteredTexelFetch(opaqueColorTexture, coordinate).rgb;
    float complTotalAlpha = filteredTexelFetch(totalAlphaTexture, coordinate).r;
    vec4 transparentColor = filteredTexelFetch(transparentColorTexture, coordinate);

    if (transparentColor.a != 0.0)
        fragColor = opaqueColor * complTotalAlpha + transparentColor.rgb * ((1.0 - complTotalAlpha) / transparentColor.a);
    else
        fragColor = opaqueColor;
}
