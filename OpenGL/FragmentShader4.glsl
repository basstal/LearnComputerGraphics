#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D texture_specular1;
    sampler2D texture_diffuse1;
    sampler2D texture_reflection1;
    samplerCube texture1;
    float shininess;
};


// struct DirLight
// {
//     vec3 direction;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// uniform DirLight dirLight;

uniform Material material;

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;

// uniform sampler2D texture1;
// uniform samplerCube texture0;

uniform vec3 cameraPos;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

// vec3 CalcDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
// {
//     vec3 lightDir = normalize( - dirLight.direction);

//     float diff = max(dot(normal, lightDir), 0.0f);

//     vec3 reflectDir = reflect(- lightDir, normal);

//     float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

//     vec3 ambient = dirLight.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
//     vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
//     vec3 specular = dirLight.specular * spec * vec3(texture(material.texture_specular1, TexCoord));


//     vec3 R = reflect(- viewDir, normalize(normal));

//     vec3 originReflec = vec3(texture(material.texture1, R).rgb);

//     return ambient + diffuse + specular + originReflec * specular;
// }

void main()
{
    // vec4 texColor = texture(texture1, TexCoord);
    // if (texColor.a < 0.1f)
    //     discard;
    // FragColor = texColor;

    // FragColor = texture(texture1, TexCoord);
    
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0f);

    vec3 viewDir = normalize(cameraPos - Position);
    vec3 normal = normalize(Normal);
    // float ratio = 1.0f / 1.52f;
    vec3 R = reflect(- viewDir, normal);
    vec3 reflectMap = vec3(texture(material.texture_reflection1, TexCoord));
    vec3 reflection = vec3(texture(material.texture1, R).rgb) * reflectMap;
    

    // vec3 originReflec = vec3(texture(material.texture1, R).rgb);
    // vec4 reflection = vec4(originReflec, 1.0f);

    float diff = max(normalize(dot(normal,  viewDir)), 0.0f);
    vec3 diffuse = diff * vec3(texture(material.texture_diffuse1, TexCoord));

    float spec = pow(max(normalize(dot(normal, R)), 0.0f), material.shininess);
    vec3 specular = spec * vec3(texture(material.texture_specular1, TexCoord));


    // vec3 light = vec3(0.0f);
    // light += CalcDirLight(dirLight, normal, viewDir);
    // FragColor = vec4(reflection + specular  + diffuse , 1.0f);
    FragColor = vec4(diffuse + reflection + specular * vec3(0.2f), 1.0f);

    // FragColor = reflection;
}
