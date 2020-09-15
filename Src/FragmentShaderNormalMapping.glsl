#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMapping;

in VS_OUT
{
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec2 TexCoord;
}fs_in;


void main()
{
    vec3 normalMap = texture(normalMapping, fs_in.TexCoord).rgb;
    // transform normal to [-1, 1]
    normalMap = normalize(normalMap * 2.0f - 1.0f);

    vec3 color = texture(diffuseTexture, fs_in.TexCoord).rgb;
    vec3 ambient = 0.2f * color;

    vec3 lightColor = vec3(0.3f);
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(normalMap, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfWay = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfWay, normalMap),  0.0f), 64.0f);
    vec3 specular = spec * lightColor;

    FragColor = vec4(diffuse + specular + ambient, 1.0f);
}