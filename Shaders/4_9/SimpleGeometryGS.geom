#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
}gs_in[];

out vec3 gs_Color;

void BuildHouse(vec4 position)
{
    gs_Color = gs_in[0].color;
    gl_Position = position + vec4(-0.2, -0.2, 0, 0);
    EmitVertex();
    gl_Position = position + vec4(0.2, -0.2, 0, 0);
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0, 0);
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0, 0);
    EmitVertex();
    gl_Position = position + vec4(0, 0.4, 0, 0);
    gs_Color = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}


void main()
{
    BuildHouse(gl_in[0].gl_Position);
}
