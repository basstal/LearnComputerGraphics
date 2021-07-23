#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

float offset = 1.0f / 300.0f;

// inverse color
// void main()
// {
//     FragColor = vec4(vec3(1.0 - texture(texture0, TexCoords)), 1.0);
// }

// grayscale
// void main()
// {
//     FragColor = texture(texture0, TexCoords);
//     float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//     FragColor = vec4(average, average, average, 1.0f);
// }

// kernels
void main()
{
    // edge detection
    // float kernal[9] = float[](
    //     1, 1, 1,
    //     1, -8, 1,
    //     1, 1, 1
    // );

    // blur
    float kernal[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    // narcotic adventure
    // float kernal[9] = float[](
    //     -1, -1, -1,
    //     -1, 9, -1,
    //     -1, -1, -1
    // );

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