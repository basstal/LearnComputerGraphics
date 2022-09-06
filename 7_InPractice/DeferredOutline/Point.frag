#version 330 core
out vec4 color;


in GS_OUT
{
  vec4 worldPos;
}gs_in;

void main()
{
    // if (length(gs_in.worldPos) == 0)
    // {
    //     discard;
    // }
    color = vec4(vec3(gs_in.worldPos), 1.0f);
}