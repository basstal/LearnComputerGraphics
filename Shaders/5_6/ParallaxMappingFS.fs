#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float height_scale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(depthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    vec3 color = vec3(texture(diffuseMap, texCoords));
    vec3 normal = vec3(texture(normalMap, texCoords));
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    
    // ambient
    vec3 ambient = 0.05 * color;
    
    // diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), 32.0);
    vec3 specular = spec * vec3(0.3); // assuming bright white light color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}
