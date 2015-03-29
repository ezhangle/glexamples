#version 140
#extension GL_ARB_explicit_attrib_location : require

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormal;
layout (location = 2) out vec4 fragWorld;

uniform mat4 modelView;
uniform mat3 normalMatrix;
uniform sampler2D boxTexture;

flat in vec3 v_normal;
smooth in vec2 v_uv;
in vec3 v_worldCoord;

void main()
{
    vec3 vertex = vec3(modelView * vec4(v_worldCoord, 1.0));
    vec3 lightPos = (modelView * vec4(0.0, 0.0, 2.0, 1.0)).xyz;
    vec3 lightDir = lightPos - vertex;
    vec3 lightColor = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 eyeVec = -vertex;

    vec3 N = normalize(normalMatrix * v_normal);
    vec3 L = normalize(lightDir);
    float lambertTerm = dot(N,L);
    
    vec3 color = texture2D(boxTexture, v_uv).rgb;
    
    float ambient = 0.6;
    
    color *= ambient;
    
    if (lambertTerm > 0.0)
    {
        color += lambertTerm * lightColor * color;
    }
    
    fragColor = vec4(color, 1.0);
    fragNormal = vec4(v_normal, 1.0);
    fragWorld = vec4(v_worldCoord, 1.0);
}
