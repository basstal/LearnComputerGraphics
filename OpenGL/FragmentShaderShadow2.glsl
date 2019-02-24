#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

in VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragLightSpacePos;
}  fs_in;

float CalculateShadow(vec4 FragLightSpacePos, float bias)
{
    vec3 coordProj = FragLightSpacePos.xyz / FragLightSpacePos.w;
    coordProj = coordProj * 0.5 + 0.5;

    float currentDepth = coordProj.z;
    if (currentDepth > 1.0f)
        return 0.0f;

        
    // float closestDepth = texture(shadowMap, coordProj.xy).r;

    // float shadow =  currentDepth  - bias> closestDepth ? 1.0 : 0.0;
    // return shadow;


    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float totalDepth = 0.0f;
    for (int x = -1; x <= 1 ; ++x)
    {
        for (int y = -1; y <= 1 ; ++ y)
        {
            float closestDepth = texture(shadowMap, coordProj.xy + vec2(x, y) * texelSize).r;
            totalDepth +=  currentDepth  - bias> closestDepth ? 1.0 : 0.0;
        }
    }
    
    return totalDepth / 9;
}

void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoord).rgb;
    vec3 ambient = color * 0.3f;
    vec3 lightColor = vec3(0.3f);

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfWay = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfWay, normal), 0.0f), 64.0f);
    vec3 specular = spec * lightColor;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = CalculateShadow(fs_in.FragLightSpacePos, bias);

    vec3 lighting = (ambient + (1 - shadow) * (specular + diffuse)) * color;
    FragColor = vec4(lighting, 1.0f);

    // float colorDepth = texture(texture0, TexCoord).r;
    // FragColor = vec4(vec3(colorDepth), 1.0f);
}