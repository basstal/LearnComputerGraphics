#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};
struct PointLight
{
    vec3 position;

    vec3 specular;
    vec3 ambient;
    vec3 diffuse;

    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLight;

uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;



vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.texture_specular1, TexCoords)));

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.texture_diffuse1, TexCoords)));

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 pointColor = CalcPointLight(pointLight, normal, FragPos, viewDir);
    FragColor = vec4(pointColor, 1.0);
}