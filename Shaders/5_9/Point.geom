#version 330 core
layout (triangles) in;
layout (points, max_vertices = 3) out;



in VS_OUT
{
    mat4 projection;
    vec3 normal;
    vec3 position;
}gs_in[];


void generatePoint(int index)
{
    gl_Position = gs_in[index].projection * gl_in[index].gl_Position;
    EmitVertex();

    EndPrimitive();
}

void main()
{
    generatePoint(0);
    generatePoint(1);
    generatePoint(2);
}
