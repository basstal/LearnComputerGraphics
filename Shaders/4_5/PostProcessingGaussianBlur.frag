#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

float offset = 1.0f / 300.0f;

void main()
{
    // Gaussian blur 3 * 3
    float kernal[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0, offset),
        vec2(offset, offset),
        vec2(-offset, 0),
        vec2(0, 0),
        vec2(offset, 0),
        vec2(-offset, -offset),
        vec2(0, -offset),
        vec2(offset, -offset)
    );
    vec3 sampleTex[9];
    for (int i = 0; i < 9; ++i)
    {
        sampleTex[i] = vec3(texture(texture0, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0f);
    for (int i = 0 ; i < 9 ; ++i)
    {
        col += kernal[i] * sampleTex[i];
    }

    FragColor = vec4(col, 1.0f);
}