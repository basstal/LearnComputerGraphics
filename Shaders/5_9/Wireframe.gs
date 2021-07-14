#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    mat4 projection;
}gs_in[];

void generate_frame(int index)
{
    gl_Position = gs_in[index].projection * gl_in[index].gl_Position;
    EmitVertex();

    if (index == 2)
    {
        gl_Position = gs_in[index].projection * gl_in[0].gl_Position;
    }
    else
    {
        gl_Position = gs_in[index].projection * gl_in[index + 1].gl_Position;
    }
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generate_frame(0);
    generate_frame(1);
    generate_frame(2);
}