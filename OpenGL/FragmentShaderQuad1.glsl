#version 330 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPos;

in vec2 TexCoord;

const int NR_LIGHT = 32;

struct Light
{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

uniform Light lights[NR_LIGHT];

void main()
{
    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = texture(gNormal, TexCoord).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoord).rgb;
    float Specular = texture(gAlbedoSpec, TexCoord).a;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_LIGHT ; ++i)
    {
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = lights[i].Color * diff * Diffuse;

        vec3 halfWay = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfWay, Normal), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;

        float distance = length(lightDir);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        lighting += diffuse + specular;
    }
    FragColor = vec4(lighting, 1.0);
}
