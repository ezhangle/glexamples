#version 140
#extension GL_ARB_explicit_attrib_location : require

in vec2 v_uv;

layout (location = 0) out vec3 fragColor;

uniform sampler2DMS opaqueColorTexture;
uniform sampler2DMS totalAlphaTexture;
uniform sampler2DMS transparentColorTexture;
uniform sampler2DMS depthTexture;


const int numSamples = 8;

vec4 filteredTexelFetch(in sampler2DMS texture, in ivec2 coordinate)
{
    vec4 texelSum = vec4(0.0);

    for (int i = 0; i < numSamples; ++i)
        texelSum += texelFetch(texture, coordinate, i);

    return texelSum / float(numSamples);
}

vec3 dimmedOpaqueColor(in ivec2 coordinate)
{
    vec3 opaqueColor = vec3(0.0);

    for (int i = 0; i < numSamples; ++i)
    {
        vec3 sampleColor = texelFetch(opaqueColorTexture, coordinate, i).rgb;
        float totalAlpha = texelFetch(totalAlphaTexture, coordinate, i).r;
        opaqueColor += sampleColor * (1.0 - totalAlpha);
    }

    return opaqueColor / float(numSamples);
}

void main()
{
    ivec2 coordinate = ivec2(gl_FragCoord.xy);

    vec3 opaqueColor = dimmedOpaqueColor(coordinate);
    float totalAlpha = filteredTexelFetch(totalAlphaTexture, coordinate).r;
    vec4 transparentColor = filteredTexelFetch(transparentColorTexture, coordinate);
    float depth = filteredTexelFetch(depthTexture, coordinate).r;

    if (transparentColor.a != 0.0)
        fragColor = opaqueColor + transparentColor.rgb * (totalAlpha / transparentColor.a);
    else
        fragColor = opaqueColor;

    gl_FragDepth = depth;
}
