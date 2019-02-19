#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// in VS_OUT
// {
//     vec3 color;
// } gs_in[];

in VS_OUT
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} gs_in[];


out GS_OUT
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} gs_out;

uniform float time;

vec4 explosion(vec4 position, vec3 normal)
{
    float magnitude = 2.0f;
    // vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude;
    return position + vec4(normal, 0.0f);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main()
{
    vec3 normal = GetNormal();
    gl_Position = explosion(gl_in[0].gl_Position, normal);
    gs_out.TexCoord = gs_in[0].TexCoord;
    gs_out.Position = gs_in[0].Position;
    gs_out.Normal = gs_in[0].Normal;
    EmitVertex();
    gl_Position = explosion(gl_in[1].gl_Position, normal);
    gs_out.TexCoord = gs_in[1].TexCoord;
    gs_out.Position = gs_in[1].Position;
    gs_out.Normal = gs_in[1].Normal;
    EmitVertex();
    gl_Position = explosion(gl_in[2].gl_Position, normal);
    gs_out.TexCoord = gs_in[2].TexCoord;
    gs_out.Position = gs_in[2].Position;
    gs_out.Normal = gs_in[2].Normal;
    EmitVertex();
    EndPrimitive();

    // aColor = gs_in[0].color;

    // gl_Position = gl_in[0].gl_Position + vec4( -0.5f, -0.5f, 0.0f, 0.0f);
    // EmitVertex();

    // gl_Position = gl_in[0].gl_Position + vec4( 0.5f, -0.5f, 0.0f, 0.0f);
    // EmitVertex();

    // gl_Position = gl_in[0].gl_Position + vec4(-0.5f, 0.5f, 0.0f, 0.0f);
    // EmitVertex();

    // gl_Position = gl_in[0].gl_Position + vec4(0.5f, 0.5f, 0.0f, 0.0f);
    // EmitVertex();

    // gl_Position = gl_in[0].gl_Position + vec4(0.0f, 1.0f, 0.0f, 0.0f);
    // aColor = vec3(1.0f, 1.0f, 1.0f);
    // EmitVertex();

    // EndPrimitive();


}