#version 330 core
out vec4 FragColor;


struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shiniess;
};

uniform Material material;

struct Light
{
    vec3 position;

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

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    vec3 cameraDir = normalize(- fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(cameraDir, reflectDir), 0.0f), material.shiniess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0f);
}