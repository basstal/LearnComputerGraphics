#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
    vec2 TexCoords;
    vec3 Normal;
    mat4 projection;
}gs_in[];

const float MAGNITUDE = 0.1;

void GenerateLine(int index)
{
    gl_Position = gs_in[index].projection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gs_in[index].projection * (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
