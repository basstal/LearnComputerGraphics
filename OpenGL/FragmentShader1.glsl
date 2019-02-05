#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // uniform 不需要 in 
in vec2 textureCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float transparent;

void main()
{
	// FragColor = texture(texture1, textureCoord / 2);
	vec2 texture2Coord = vec2(- textureCoord.x, textureCoord.y);
	FragColor = mix(texture(texture1, textureCoord), texture(texture2, texture2Coord), transparent);
	// FragColor = texture(texture1, textureCoord / 2) * texture(texture2, textureCoord);
	// FragColor = texture(texture1, textureCoord);
}
