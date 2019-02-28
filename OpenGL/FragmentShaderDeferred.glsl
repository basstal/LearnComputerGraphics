#version 330 core
layout (location = 0) out vec3 OPosition;
layout (location = 1) out vec3 ONormal;
layout (location = 2) out vec4 OAlbedoSpec;

in VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
}fs_in;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main()
{
    OPosition = fs_in.FragPos;

    ONormal = normalize(fs_in.Normal);

    OAlbedoSpec.rgb = texture(material.texture_diffuse1, fs_in.TexCoord).rgb;

    OAlbedoSpec.a = texture(material.texture_specular1, fs_in.TexCoord).r;
}
