#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; // uniform 不需要 in 
in vec4 vertexColor;

void main()
{
	FragColor = vertexColor;
}
