#version 330 core
out vec4 FragColor;


struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D colorSpecular;
    sampler2D emission;
    float shininess;
};

uniform Material material;

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;
    

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 direction;

    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

uniform SpotLight spotLight;

#define POINT_LIGHT_COUNT 4
uniform PointLight pointLights[POINT_LIGHT_COUNT];

// uniform vec3 objectColor;

// uniform vec3 lightPos;
// uniform vec3 cameraPos;

uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoord;

vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize( - dirLight.direction);

    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(- lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    vec3 ambient = dirLight.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = dirLight.specular * spec * vec3(texture(material.specular, TexCoord));

    return ambient + diffuse + specular;
}


vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0f);

    vec3 reflectDir = reflect(- lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    float distance = length(fragPos - pointLight.position);
    float attenuation = 1 / (distance * pointLight.linear + distance * distance * pointLight.quadratic + pointLight.constant);

    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, TexCoord));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float diff = max(dot(normal, viewDir), 0.0f);

    vec3 reflectDir = reflect(- viewDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    float theta = dot(viewDir, normalize( - spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0f, 1.0f);

    vec3 ambient = spotLight.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = spotLight.specular * spec * vec3(texture(material.specular, TexCoord));

    diffuse *= intensity;
    specular *= intensity;
    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
    {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    }

    result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

    // result += vec3(texture(material.emission, TexCoord));

    FragColor = vec4(result, 1.0f);

    // vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // vec3 norm = normalize(normal);
    // float distance = length(light.position - fragPos);
    // float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // vec3 lightDir = normalize(light.position - fragPos);
    // float diff = max(dot(norm, lightDir), 0.0f);
    // vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // vec3 cameraDir = normalize(- fragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(cameraDir, reflectDir), 0.0f), material.shininess);
    // vec3 origin = vec3(texture(material.specular, TexCoord));
    // vec3 inverse = vec3(1.0f - origin.x, 1.0f - origin.y, 1.0f - origin.z);
    // vec3 colorSpecular = vec3(texture(material.colorSpecular, TexCoord));

    // vec3 specular = light.specular * spec * origin;

    // vec3 emission = vec3(texture(material.emission, TexCoord));

    // // 手电筒
    // float theta = dot(lightDir, normalize(- light.direction));
    // float epsilon = light.cutOff - light.outerCutOff;
    // float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    // diffuse *= intensity;
    // specular *= intensity;
    // vec3 result = ( ambient + diffuse + specular ) * attenuation + emission;
    // // vec3 result = ( ambient + diffuse + specular ) * attenuation + emission;
    // FragColor = vec4(result, 1.0f);
}