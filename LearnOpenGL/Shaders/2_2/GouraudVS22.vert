#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outColor;

void main()
{
    vec4 FragPos = view * model * vec4(aPos, 1.0);
    gl_Position = projection * FragPos;
    vec3 Normal = normalize(mat3(transpose(inverse(view * model))) * aNormal);
    vec3 FragPosView = vec3(FragPos);
    vec3 lightPosView = vec3(view * vec4(lightPos, 1.0));


    vec3 lightDir = normalize(lightPosView - FragPosView);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.7;
    vec3 viewDir = normalize(-FragPosView);
    float spec = pow(max(dot(reflect(-lightDir, Normal), viewDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    outColor = (ambient + specular + diffuse) * objectColor;
}
