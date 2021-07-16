#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform bool backFaceCull;
uniform vec3 viewPos;

in GS_OUT
{
    vec3 Normal;
    vec3 Position;
    mat4 model;
}fs_in;

void main()
{
    if (backFaceCull)
    {
        vec3 viewPosModel = vec3(inverse(fs_in.model) * vec4(viewPos, 1.0));
        vec3 V = normalize(fs_in.Position - viewPosModel);
        // V = vec3(inverse(fs_in.model) * vec4(V, 1.0));
        if (dot(V, fs_in.Normal) > 0)
        {
            discard;
        }
    }
    FragColor = vec4(color, 1.0f);
}
