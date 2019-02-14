#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D texture1;

float near = 0.1f;
float far = 100.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0 * near * far) / (far + near - z * (far - near));
}
void main()
{
    vec4 texColor = texture(texture1, texCoord);
    if (texColor.a < 0.1f)
        discard;
    FragColor = texColor;
    
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0f);
}