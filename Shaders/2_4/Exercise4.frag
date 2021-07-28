#version 330 core
out vec4 FragColor;
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

in vec2 TexCoords;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

void main()
{    
    vec3 diffuseColor = vec3(texture(material.diffuse, TexCoords));
    // ambient
    vec3 ambient = light.ambient * diffuseColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularColor = vec3(texture(material.specular, TexCoords));
    vec3 specular = light.specular * spec * specularColor;

    // emission
    vec3 emission = vec3(texture(material.emission, TexCoords));
        
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}