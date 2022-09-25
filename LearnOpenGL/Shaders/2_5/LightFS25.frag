#version 330 core
out vec4 FragColor;
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec2 TexCoords;

struct Light {
    // ** common
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // ** spot light & point light
    vec3 position;

    // ** spot light & diretion light
    vec3 direction;

    // ** spot light
    float cutOff;
    float outerCutOff;

    // ** Point Light
    float constant;
    float linear;
    float quadratic;
};

uniform Light spotLight;
uniform Light pointLight;
uniform Light directionLight;

uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

vec3 AmbientColor(vec3 inAmbient)
{
    // ambient
    vec3 ambient = inAmbient * vec3(texture(material.diffuse, TexCoords));
    return ambient;
}

vec3 SpecularColor(vec3 lightDir, vec3 inSpecular)
{
    // specular
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = inSpecular * (spec * vec3(texture(material.specular, TexCoords)));
    return specular;
}

vec3 DiffuseColor(vec3 lightDir, vec3 inDiffuse)
{
    // diffuse 
    
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = inDiffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    return diffuse;
}

vec3 CalculateSpotLight()
{
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = AmbientColor(spotLight.ambient);
    vec3 diffuse = DiffuseColor(lightDir, spotLight.diffuse);
    vec3 specular = SpecularColor(lightDir, spotLight.specular);
    
    // ** Spot Light
    diffuse *= intensity;
    specular *= intensity;
    return ambient + diffuse + specular;
}

vec3 CalculatePointLight()
{
    // ** Point Light
    float dist    = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));   
    
    vec3 ambient = AmbientColor(pointLight.ambient);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    vec3 diffuse = DiffuseColor(lightDir, pointLight.diffuse);
    vec3 specular = SpecularColor(lightDir, pointLight.specular);
    
    
    // ** Point Light
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}

vec3 CalculateDirectionLight()
{
    vec3 ambient = AmbientColor(directionLight.ambient);
    vec3 diffuse = DiffuseColor(directionLight.direction, directionLight.diffuse);
    vec3 specular = SpecularColor(directionLight.direction, directionLight.specular);
    return ambient + diffuse + specular;
}
void main()
{    
    vec3 pointLight = CalculatePointLight();
    vec3 spotLight = CalculateSpotLight();
    vec3 directionLight = CalculateDirectionLight();
    FragColor = vec4(directionLight + spotLight + pointLight, 1.0);
}