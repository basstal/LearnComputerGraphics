#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform samplerCube shadowCubeMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

in VS_OUT
{
    vec2 TexCoord;
    vec3 FragPos;
    vec3 Normal;
}  fs_in;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float CalculateShadow(vec3 FragPos, float bias)
{
    vec3 fragToLight = FragPos - lightPos;
    float currentDepth = length(fragToLight);
    // float closestDepth = texture(shadowCubeMap, fragToLight).r;
    // closestDepth *= far_plane;

    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // return shadow;
    float viewDistance = length(FragPos - viewPos);
    float shadow = 0.0f;
    int samples = 20;
    float diskRadians = (1.0 + viewDistance / far_plane ) / 25.0f;
    for (int i = 0; i < samples ; ++ i )
    {
        float closestDepth = texture(shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadians).r;
        closestDepth *= far_plane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0f;
    }
    shadow /= samples;
    return shadow;
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
    float shadow = CalculateShadow(fs_in.FragPos, bias);

    vec3 lighting = (ambient + (1 - shadow) * (specular + diffuse)) * color;
    FragColor = vec4(lighting, 1.0f);

    // float colorDepth = texture(texture0, TexCoord).r;
    // FragColor = vec4(vec3(colorDepth), 1.0f);
}