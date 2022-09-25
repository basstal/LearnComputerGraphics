#version 330 core
out vec4 FragColor;

int offset = 5;

uniform bool bUsePostProcess;
uniform sampler2DMS screenTextureMS;

void main()
{
    vec3 col = vec3(0.0f);
    if (bUsePostProcess)
    {
        // blur
        float kernal[9] = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16
        );
        ivec2 offsets[9] = ivec2[](
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
            sampleTex[i] = vec3(texelFetch(screenTextureMS, ivec2(gl_FragCoord.xy) + offsets[i], 3));
        }
        for (int i = 0 ; i < 9 ; ++i)
        {
            col += kernal[i] * sampleTex[i];
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            col += vec3(texelFetch(screenTextureMS, ivec2(gl_FragCoord.xy), i));
        }
        col /= 8;
    }
    FragColor = vec4(col, 1.0);
}