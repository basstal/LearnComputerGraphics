#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
    vec3 color;
} gs_in[];

out vec3 aColor;
 
void main()
{
    aColor = gs_in[0].color;

    gl_Position = gl_in[0].gl_Position + vec4( -0.5f, -0.5f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.5f, -0.5f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.5f, 0.5f, 0.0f, 0.0f);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.0f, 1.0f, 0.0f, 0.0f);
    aColor = vec3(1.0f, 1.0f, 1.0f);
    EmitVertex();

    EndPrimitive();
}