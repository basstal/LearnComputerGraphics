#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // uniform 不需要 in 

void main()
{
	FragColor = vertexColor;
}
