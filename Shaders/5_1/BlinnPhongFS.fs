#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
}fs_in;

uniform vec3 viewPos;
uniform float shininess;
uniform bool openBlinn;

struct Light
{
    vec3 position;

    float ambient;
    float diffuse;
    float specular;
};

uniform Light light;

void main()
{
    // vec3 lightDir = normalize(light.position - fs_in.FragPos);
    // vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // // ambient
    // vec3 ambient = light.ambient * vec3(texture(tex, fs_in.TexCoords));
    
    // // diffuse 
    // vec3 norm = normalize(fs_in.Normal);
    // float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = light.diffuse * (diff * vec3(texture(tex, fs_in.TexCoords)));
    
    // // specular
    // vec3 specular;

    // if (openBlinn)
    // {
    //     vec3 halfwayDir = normalize(lightDir + viewDir);
    //     float spec = pow(max(dot(halfwayDir, norm), 0.0), shininess);
    //     specular = light.specular * (spec * vec3(texture(tex, fs_in.TexCoords)));
    // }
    // else
    // {
    //     vec3 reflectDir = reflect(-lightDir, norm);
    //     float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    //     specular = light.specular * (spec * vec3(texture(tex, fs_in.TexCoords)));
    // }

    // vec3 result = ambient + diffuse + specular;
    // FragColor = vec4(result, 1.0);

    FragColor = texture(tex, fs_in.TexCoords);
}