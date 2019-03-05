#version 330 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gAmbientOcclusion;

in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float linear;
uniform float quadratic;

uniform bool enableSSAO;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoord).rgb;
    vec3 Normal = texture(gNormal, TexCoord).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoord).rgb;
    float AmbientOcclusion = texture(gAmbientOcclusion, TexCoord).r;

    vec3 lighting = Diffuse * 0.2;
    if (enableSSAO)
        lighting *= AmbientOcclusion;
    vec3 viewDir = normalize( - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * Diffuse;

    vec3 halfWay = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfWay, Normal), 0.0), 16.0);
    vec3 specular = lightColor * spec;

    float distance = length(lightDir);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;

    lighting += diffuse + specular;
    FragColor = vec4(lighting, 1.0);
}
