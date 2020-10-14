#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};
uniform Material material;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
}fs_in;

void main()
{
    FragColor = texture(material.texture_diffuse1, fs_in.TexCoords);
}