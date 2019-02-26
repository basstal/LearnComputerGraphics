#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapping;
uniform sampler2D parallaxMapping;

in VS_OUT
{
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightPos;
    vec3 TangentNormal;
    vec2 TexCoord;
    vec3 FragPos;
}fs_in;

uniform bool enableNormalMap;

uniform float heightScale;

vec2 CalcParallaxMapping(vec2 TexCoord, vec3 viewDir)
{
    float height = texture(parallaxMapping, TexCoord).r;
    vec2 p = viewDir.xy * (height * heightScale);
    return TexCoord - p;
    // return TexCoord;
}

void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoord = CalcParallaxMapping(fs_in.TexCoord, viewDir);

    if (texCoord.x < 0 || texCoord.y < 0 || texCoord.x > 1 || texCoord.y > 1)
        discard;
    
    vec3 normal = fs_in.TangentNormal;
    if (enableNormalMap)
    {
        normal = texture(normalMapping, texCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    vec3 color = texture(diffuseTexture, texCoord).rgb;
    vec3 ambient = 0.5f * color;

    vec3 light = vec3(0.2f);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max( dot(lightDir, normal), 0.0);
    vec3 diffuse = light * diff;

    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow( max( dot(halfWay, normal), 0.0 ), 32.0f);
    vec3 specular = light * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}