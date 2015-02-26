#version 150 core
#extension GL_ARB_sample_shading

in vec3 v_normal;

out vec4 fragColor;

uniform sampler2D noise;
uniform vec2 viewport;

void main()
{
    float[] thresholdMatrix = float[16](
        1.0 / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0,
        13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0,
        4.0 / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0,
        16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0
    );

    int index = int(mod(floor(gl_FragCoord.x), 4) + mod(floor(gl_FragCoord.y), 4) * 4);
    float threshold = thresholdMatrix[index];

    if ((threshold) > 0.4)
        discard;

	fragColor = vec4(v_normal * 0.5 + 0.5, 1.0);
}
