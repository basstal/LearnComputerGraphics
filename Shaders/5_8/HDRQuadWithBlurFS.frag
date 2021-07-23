#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texHDR;
uniform sampler2D bloomBlur;

vec4 ReinhardToneMapping()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(texHDR, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;

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
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor += bloomColor;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    return vec4(mapped, 1.0);
}  

void main()
{
    // vec3 hdrColor = texture(texHDR, TexCoords).rgb;
    // FragColor = vec4(hdrColor, 1.0);

    // FragColor = ReinhardToneMapping();

    FragColor = ExposureToneMapping();
}