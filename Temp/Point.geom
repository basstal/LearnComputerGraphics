#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 13) out;


in VS_OUT{
    float legalPoints;
}gs_in[];


void drawByIndex(int index)
{
    if (gs_in[index].legalPoints > 0)
    {
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position + vec4(vec3(0.02f, 0.0f, 0.0f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position + vec4(vec3(0.0f, 0.02f, 0.0f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position + vec4(vec3(0.0f, 0.0f, 0.02f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position - vec4(vec3(0.02f, 0.0f, 0.0f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position - vec4(vec3(0.0f, 0.02f, 0.0f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();
        gl_Position = gl_in[index].gl_Position - vec4(vec3(0.0f, 0.0f, 0.02f), 0.0f);
        EmitVertex();
        gl_Position = gl_in[index].gl_Position;
        EmitVertex();

        EndPrimitive();
    }
}
void main()
{
    drawByIndex(0);
    drawByIndex(1);
    drawByIndex(2);
}