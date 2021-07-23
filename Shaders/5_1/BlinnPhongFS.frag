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
uniform bool openBlinn;
uniform vec3 lightPos;

void main()
{
    vec3 color = vec3(texture(tex, fs_in.TexCoords));

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // ambient
    vec3 ambient = 0.05 * color;
    
    // diffuse 
    vec3 norm = normalize(fs_in.Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    float spec = 0.0;
    if (openBlinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, norm), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * vec3(0.3); // assuming bright white light color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}