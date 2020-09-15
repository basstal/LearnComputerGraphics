#version 330 core
out vec4 FragColor;

uniform sampler2D hdrTexture;

const float gamma = 2.2;

in VS_OUT
{
    vec2 TexCoord;
    // vec3 Normal;
}fs_in;

uniform float exposure;

void main()
{
    vec3 colorHDR = texture(hdrTexture, fs_in.TexCoord).rgb;

    vec3 color = vec3(1.0f) - exp(-colorHDR * exposure);

    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0f);
}