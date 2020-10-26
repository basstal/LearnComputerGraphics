#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}fs_in;

struct Light
{
    vec3 position;
    vec3 color;
};

uniform Light lights[4];

uniform vec3 viewPos;
uniform sampler2D texture0;

uniform float constant;
uniform float linear;
uniform float quadratic;
uniform float shininess;
uniform float ambientStrength;

vec3 ApplyLight(Light light, vec3 normal)
{
    vec3 lightColor = light.color;
    // ambient
    vec3 ambient = ambientStrength * lightColor;
    // diffuse
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * lightColor;

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main()
{

    vec3 color = texture(texture0, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    vec3 finalColor = vec3(0);
    for (int i = 0; i < 4; ++i)
    {
        finalColor += ApplyLight(lights[i], normal);
    }
    finalColor *= color;
    FragColor = vec4(finalColor, 1.0);
}