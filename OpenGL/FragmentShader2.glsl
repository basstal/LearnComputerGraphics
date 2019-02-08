#version 330 core
out vec4 FragColor;


struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D colorSpecular;
    sampler2D emission;
    float shiniess;
};

uniform Material material;

struct Light
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
    

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
// uniform vec3 objectColor;

// uniform vec3 lightPos;
// uniform vec3 cameraPos;

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoord;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    vec3 norm = normalize(normal);
    float distance = length(light.position - fragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    vec3 cameraDir = normalize(- fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(cameraDir, reflectDir), 0.0f), material.shiniess);
    vec3 origin = vec3(texture(material.specular, TexCoord));
    vec3 inverse = vec3(1.0f - origin.x, 1.0f - origin.y, 1.0f - origin.z);
    vec3 colorSpecular = vec3(texture(material.colorSpecular, TexCoord));

    vec3 specular = light.specular * spec * origin;

    vec3 emission = vec3(texture(material.emission, TexCoord));

    // 手电筒
    float theta = dot(lightDir, normalize(- light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;
    vec3 result = ( ambient + diffuse + specular ) * attenuation + emission;
    // vec3 result = ( ambient + diffuse + specular ) * attenuation + emission;
    FragColor = vec4(result, 1.0f);
}