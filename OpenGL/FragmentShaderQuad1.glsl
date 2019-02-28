#version 330 core
out vec4 FragColor;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedoSpec;

uniform vec3 viewPos;

in vec2 TexCoord;

const int NR_LIGHT = 32;

struct Light
{
    vec3 position;
    vec3 color;

    float Linear;
    float Quadratic;
};

Light lights[NR_LIGHT];

void main()
{
    vec3 FragPos = texture(position, TexCoord).rgb;
    vec3 Normal = texture(normal, TexCoord).rgb;
    vec3 Diffuse = texture(albedoSpec, TexCoord).rgb;
    float Specular = texture(albedoSpec, TexCoord).a;

    vec3 lighting = Diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_LIGHT ; ++i)
    {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = lights[i].color * diff * Diffuse;

        vec3 halfWay = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfWay, Normal), 0.0), 32.0);
        vec3 specular = lights[i].color * spec * Specular;

        float distance = length(lightDir);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        lighting += diffuse + specular;
    }
    FragColor = vec4(lighting, 1.0);
}
