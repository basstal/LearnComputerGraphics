#version 330 core
out float FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTex;

uniform vec3 samples[64];
uniform mat4 projection;

uniform float radians;
uniform float bias;

const vec2 texNoiseScale = vec2(1280/4.0, 780/4.0);

in vec2 TexCoord;

void main()
{
    vec3 FragPos = texture(gPosition, TexCoord).xyz;
    vec3 Normal = texture(gNormal, TexCoord).rgb;
    vec3 randomVec = texture(noiseTex, TexCoord * texNoiseScale).rgb;

    // gramm-schmdit process
    vec3 tangent = normalize(randomVec - Normal * dot(randomVec, Normal));
    vec3 bitangent = cross(tangent, Normal);
    mat3 TBN = mat3(tangent, bitangent, Normal);
    
    float occlusion = 0.0;

    for (int i = 0 ; i < 64 ; ++ i)
    {
        vec3 sample = TBN * samples[i];
        sample = FragPos + sample * radians;
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radians / abs(FragPos.z - sampleDepth));
        occlusion += (sampleDepth > sample.z + bias ? 1.0f : 0.0f) * rangeCheck;
    }
    FragColor = 1.0 - (occlusion / 64);

}