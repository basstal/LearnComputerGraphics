#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
}fs_in;

void main()
{
    gPosition = fs_in.FragPos;

    gNormal = normalize(fs_in.Normal);

    gAlbedoSpec.rgb = vec3(0.95);
}