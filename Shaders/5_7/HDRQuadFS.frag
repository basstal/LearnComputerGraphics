#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texHDR;

vec4 ReinhardToneMapping()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(texHDR, TexCoords).rgb;
  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    return vec4(mapped, 1.0);
}

uniform float exposure;

vec4 ExposureToneMapping()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(texHDR, TexCoords).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    return vec4(mapped, 1.0);
}  

void main()
{
    FragColor = ExposureToneMapping();
}