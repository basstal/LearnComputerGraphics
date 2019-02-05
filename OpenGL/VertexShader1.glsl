#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 textureCoord;

// uniform float offset;
uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 rotateByTime;

uniform mat4 newModel;

void main()
{
	gl_Position = projection * view * newModel * vec4(aPos, 1.0f);
	vertexColor = vec4(aColor, 1.0f);
	textureCoord = aTexCoord;
}