#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture0;

float offset = 1.0f / 300.0f;

void main()
{
    FragColor = texture(texture0, texCoord);
    // FragColor = texture(texture0, texCoord);
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average, average, average, 1.0f);

    // post-processing
    // vec2 offsets[9] = vec2[](
    //     vec2(-offset, offset),
    //     vec2(0, offset),
    //     vec2(offset, offset),
    //     vec2(-offset, 0),
    //     vec2(0, 0),
    //     vec2(offset, 0),
    //     vec2(-offset, -offset),
    //     vec2(0, -offset),
    //     vec2(offset, -offset)
    // );

    // float narcoticAdventure[9] = float[](
    //     -1, -1, -1,
    //     -1, 9, -1,
    //     -1, -1, -1
    // );

    // float blur[9] = float[](
    //     1 / 16f, 2 / 16f, 1 / 16f,
    //     2 / 16f, 4 / 16f, 2 / 16f,
    //     1 / 16f, 2 / 16f, 1 / 16f
    // );

    // float edgeDetection[9] = float[](
    //     1, 1, 1,
    //     1, -8, 1,
    //     1, 1, 1
    // );

    // vec3 sampleTex[9];

    // for (int i = 0; i < 9; ++i)
    // {
    //     sampleTex[i] = vec3(texture(texture0, texCoord.st + offsets[i]));
    // }
    // vec3 sum = vec3(0.0f);
    // for (int i = 0 ; i < 9 ; ++i)
    // {
    //     sum += edgeDetection[i] * sampleTex[i];
    // }

    // FragColor = vec4(sum, 1.0f);
}