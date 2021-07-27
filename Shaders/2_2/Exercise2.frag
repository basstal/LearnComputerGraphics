#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;

in vec3 Normal;
in vec3 FragPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform int shininess;

void main()
{
    // Forgetting to normalize a vector is a popular mistake.
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    vec3 viewDir = normalize(- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 ambient = ambientStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}