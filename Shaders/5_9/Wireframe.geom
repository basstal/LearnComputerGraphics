#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    mat4 projection;
    mat4 model;
    vec3 Position;
}gs_in[];

out GS_OUT
{
    vec3 Normal;
    vec3 Position;
    mat4 model;
}gs_out;

void generate_frame(int index)
{
    gl_Position = gs_in[index].projection * gl_in[index].gl_Position;
    gs_out.Normal = gs_in[index].Normal;
    gs_out.Position = gs_in[index].Position;
    gs_out.model = gs_in[index].model;
    EmitVertex();

    if (index == 2)
    {
        gl_Position = gs_in[index].projection * gl_in[0].gl_Position;
    }
    else
    {
        gl_Position = gs_in[index].projection * gl_in[index + 1].gl_Position;
    }
    gs_out.Normal = gs_in[index].Normal;
    gs_out.Position = gs_in[index].Position;
    gs_out.model = gs_in[index].model;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generate_frame(0);
    generate_frame(1);
    generate_frame(2);
}