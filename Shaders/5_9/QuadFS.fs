#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;

    float Radius;
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
uniform float shininess;
// uniform int drawMode;

void main()
{             
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);

    
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        Light light = lights[i];
        float light2FragDist = length(light.Position - FragPos);
        if (light2FragDist < light.Radius)
        {
            // diffuse
            vec3 lightDir = normalize(light.Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * light.Color;

            // specular
            vec3 reflectDir = reflect(-lightDir, Normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            vec3 specular = Specular * spec * light.Color;

            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * light2FragDist + lights[i].Quadratic * light2FragDist * light2FragDist);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }
    
    // if (drawMode == 0)
    // {
    FragColor = vec4(lighting, 1.0);
    // }
    // else
    // {
    //     FragColor = vec4(1.0f);
    // }
}  