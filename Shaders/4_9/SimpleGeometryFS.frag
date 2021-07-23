#version 330 core
out vec4 FragColor;

in vec3 gs_Color;
void main()
{
    FragColor = vec4(gs_Color, 1.0);
}