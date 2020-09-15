#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out GS_OUT
{
    vec3 color;
} gs_out;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    gs_out.color = vec3(1.0f, 0.0f, 0.0f);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    gs_out.color = vec3(0.0f, 1.0f, 0.0f);
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    gs_out.color = vec3(0.0f, 0.0f, 1.0f);
    EmitVertex();
    EndPrimitive();
}