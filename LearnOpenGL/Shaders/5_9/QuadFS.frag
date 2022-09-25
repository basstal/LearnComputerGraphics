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

void main()
{             
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    bool subjects = Normal.x != 0 || Normal.y != 0 || Normal.z != 0;
    vec3 lighting = vec3(0.0f);
    if (subjects)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        bool bLightup = false;
        for(int i = 0; i < NR_LIGHTS; ++i)
        {
            Light light = lights[i];
            float light2FragDist = length(light.Position - FragPos);
            if (light2FragDist < light.Radius)
            {
                bLightup = true;
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
        if (bLightup)
        {
            lighting += Albedo * 0.1; // hard-coded ambient component
        }

    }

    FragColor = vec4(lighting, 1.0);
}  