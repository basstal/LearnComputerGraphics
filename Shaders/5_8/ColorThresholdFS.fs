#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

struct Light
{
    vec3 color;
    vec3 position;
};

uniform Light lights[4];
uniform vec3 viewPos;
uniform sampler2D diffuseTex;

uniform float constant;
uniform float linear;
uniform float quadratic;
uniform float shininess;
uniform float ambientStrength;

vec3 CalcLight(Light light, vec3 normal, vec3 color)
{
    vec3 lightColor = light.color;
    // ambient
    vec3 ambient = ambientStrength * color;
    // diffuse
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * lightColor * color;

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 color = texture(diffuseTex, fs_in.TexCoords).rgb;

    vec3 lighting = vec3(0);
    for (int i = 0; i < 4; ++i)
    {
        lighting += CalcLight(lights[i], normal, color);
    }
    FragColor = vec4(lighting, 1.0);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}