#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapping;

in VS_OUT
{
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;


void main()
{
    vec3 color = vec3(texture(diffuseTexture, fs_in.TexCoords));

    vec3 normal = texture(normalMapping, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    // ambient
    vec3 ambient = 0.05 * color;
    
    // diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(halfwayDir, normal), 0.0), 32.0);
    vec3 specular = spec * vec3(0.3); // assuming bright white light color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}