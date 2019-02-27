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

const float minLayers = 6.0;
const float maxLayers = 32.0;

vec2 CalcParallaxMapping(vec2 TexCoord, vec3 viewDir)
{
    // steep parallax mapping
    float numberOfLayers = mix(minLayers, maxLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    vec2 p = viewDir.xy * heightScale;
    vec2 depthPerLayer = p / numberOfLayers;
    vec2 currentTexCoord = TexCoord;
    float currentDepthMapVal = texture(parallaxMapping, currentTexCoord).r;
    float layerDepth = 1.0 / numberOfLayers;
    float currentLayerDepth = 0.0;


    while( currentLayerDepth < currentDepthMapVal )
    {
        currentTexCoord -= depthPerLayer;
        currentDepthMapVal = texture(parallaxMapping, currentTexCoord).r;
        currentLayerDepth += layerDepth;
    }
    vec2 beforeTexCoord = currentTexCoord - depthPerLayer;

    float afterDepth = currentDepthMapVal - currentLayerDepth;
    float beforeDepth = texture(parallaxMapping, beforeTexCoord).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoord = weight * beforeTexCoord + (1 - weight) * currentTexCoord;
    return finalTexCoord;


    // return currentTexCoord;

    // parallax mapping
    // float height = texture(parallaxMapping, TexCoord).r;
    // vec2 p = viewDir.xy * (height * heightScale);
    // return TexCoord - p;
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
    vec3 ambient = 0.1f * color;

    vec3 light = vec3(0.2f);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max( dot(lightDir, normal), 0.0);
    vec3 diffuse = color * diff;

    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow( max( dot(halfWay, normal), 0.0 ), 32.0f);
    vec3 specular = light * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}