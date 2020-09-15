#version 330 core
out vec4 FragColor;

uniform float shininess;

uniform sampler2D texture0;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

struct DotLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DotLight dotLight;

uniform vec3 viewPos;

uniform bool openBlinn;

const float kPi = 3.14159265;

vec3 CalDotLight(DotLight light, vec3 normal)
{

    vec3 ambient = light.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    vec3 diffuse = max(dot(lightDir, normal), 0.0f) * light.diffuse;


    vec3 viewDir = normalize(viewPos - FragPos);

    if (openBlinn)
    {
        float kEnergyConservation = ( 8.0 + shininess ) / ( 8.0 * kPi ); 

        vec3 halfWay = normalize(viewDir + lightDir);
        vec3 specular = kEnergyConservation * pow(max(dot(halfWay, normal), 0.0f), shininess) * light.specular;
        return (specular + diffuse) + ambient ;
    }
    else
    {
        float kEnergyConservation = ( 2.0 + shininess ) / ( 2.0 * kPi ); 

        vec3 R = reflect( - lightDir, normal);
        vec3 specular = kEnergyConservation * pow(max(dot(viewDir, R), 0.0f), shininess) * light.specular;
        return (specular + diffuse) + ambient;
    }

}

void main()
{
    vec3 color = vec3(0.0f);

    vec3 normal = normalize(Normal);

    color += CalDotLight(dotLight, normal);

    FragColor = vec4(vec3(texture(texture0, TexCoord)) * color, 1.0f);
}