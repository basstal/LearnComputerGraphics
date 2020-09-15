#version 330 core
out vec4 FragColor;

uniform sampler2D texture0;

in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
}fs_in;

// uniform vec3 lightPos;
uniform vec3 viewPos;


struct Light
{
    vec3 color;
    vec3 position;
};

uniform Light lights[16];

vec3 CalculateSpotLight(Light light, vec3 normal)
{
    vec3 lightDir = normalize(light.position - fs_in.FragPos);

    vec3 color = texture(texture0, fs_in.TexCoord).rgb;
    vec3 ambient = 0.2f * color;

    vec3 lightColor = light.color;
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfWay = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfWay, normal), 0.0), 32.0f);
    vec3 specular = spec * lightColor;

    float distance = length(fs_in.FragPos - light.position);
    float attenuation = 1.0 / (distance * distance);

    return ( specular + diffuse + ambient ) * attenuation;
}


void main()
{
    // vec3 normal = normalize(fs_in.Normal);

    // vec3 color = vec3(0.0);
    
    // for (int i = 0; i < 4 ; ++i)
    //     color += CalculateSpotLight(light[i], normal);

    // FragColor = vec4(color, 1.0f);


    vec3 color = texture(texture0, fs_in.TexCoord).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].color * diff * color;
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - lights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
    }
    FragColor = vec4(lighting + ambient, 1.0f);
}
