#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D diffuse;

};

uniform Material material;

in vec2 TexCoords;

float near = 0.1;
float far = 100;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    FragColor = texture(material.diffuse, TexCoords);
}