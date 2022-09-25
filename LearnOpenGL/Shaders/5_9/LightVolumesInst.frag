#version 330 core
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

in VS_OUT
{
    vec3 NormalModel;
    vec3 Position;
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 lightPosition;
    vec3 lightColor;
    float lightRadius;
}fs_in;

uniform float Linear;
uniform float Quadratic;
uniform float shininess;
uniform vec2 windowSize;
uniform vec3 viewPos;

void main()
{
    mat4 inverseModel = inverse(fs_in.model);
    vec3 viewPosModel = vec3(inverseModel * vec4(viewPos, 1.0));
    vec3 V = normalize(fs_in.Position - viewPosModel);
    // ** only render sphere back face
    if (dot(V, fs_in.NormalModel) < 0)
    {
        discard;
    }

    // ** 求视线与球面的交点
    vec3 S = viewPosModel;
    float a = pow(length(V), 2);
    float b = 2 * (dot(S, V));
    float c = pow(length(S), 2) - pow(fs_in.lightRadius, 2);
    float D = pow(b, 2) - 4 * a * c;
    float sqrt_D = sqrt(D);
    float minT = (-b - sqrt_D) / (2 * a);
    float maxT = (-b + sqrt_D) / (2 * a);
    // model coordinate
    vec3 p0 = S + minT * V;
    vec3 p1 = S + maxT * V;
    
    
    vec2 samplerUV = (gl_FragCoord.xy - 0.5) / windowSize;
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, samplerUV).rgb;
    mat4 worldToNDC = fs_in.projection * fs_in.view;
    mat4 modelToNDC = worldToNDC * fs_in.model;
    vec3 FragPosNDC = vec3(worldToNDC * vec4(FragPos, 1.0));
    vec3 minP = vec3(modelToNDC * vec4(p0, 1.0));
    vec3 maxP = vec3(modelToNDC * vec4(p1, 1.0));
    if (FragPosNDC.z < minP.z || FragPosNDC.z > maxP.z)
    {
        discard;
    }
    vec3 Normal = texture(gNormal, samplerUV).rgb;
    vec3 Albedo = texture(gAlbedoSpec, samplerUV).rgb;
    float Specular = texture(gAlbedoSpec, samplerUV).a;
    bool subjects = Normal.x != 0 || Normal.y != 0 || Normal.z != 0;
    vec3 lighting = vec3(0);
    if (subjects)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        float light2FragDist = length(fs_in.lightPosition - FragPos);
        // diffuse
        vec3 lightDir = normalize(fs_in.lightPosition - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * fs_in.lightColor;

        // specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = Specular * spec * fs_in.lightColor;

        // attenuation
        float attenuation = 1.0 / (1.0 + Linear * light2FragDist + Quadratic * light2FragDist * light2FragDist);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }
    FragColor = vec4(lighting, 1.0);
}