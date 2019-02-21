#version 330 core
out vec4 FragColor;

in GS_OUT
{
    vec3 color;
} fs_in;

void main()
{
    FragColor = vec4(fs_in.color, 1.0f);
}