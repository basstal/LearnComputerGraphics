#version 330 core
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D brightness0;

uniform float exposure;

const float gamma = 2.2f;

in vec2 TexCoord;

void main()
{

    vec3 color = texture(texture0, TexCoord).rgb;
    vec3 brightness = texture(brightness0, TexCoord).rgb;
    
    color += brightness;

    vec3 result = vec3(1.0f) - exp(- color * exposure);

    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0f);
}