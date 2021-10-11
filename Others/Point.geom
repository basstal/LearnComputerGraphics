#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;


in VS_OUT{
    float legalPoints;
    vec3 worldPos;
    bool removeEdgeToPointA;
    bool removeEdgeToPointB;
    vec3 vertex;
}gs_in[];

out GS_OUT
{
  vec3 worldPos;
}gs_out;

bool checkDrawable(int firstIndex, int  nextIndex)
{
    bool hasDistantNormal = gs_in[firstIndex].legalPoints > 0 && gs_in[nextIndex].legalPoints > 0;
    bool removeEdge = gs_in[firstIndex].removeEdgeToPointA;
    return hasDistantNormal && !removeEdge;
    // return hasDistantNormal;
}
void drawByIndex(int index)
{
    gs_out.worldPos = vec3(0.0f);
    int firstIndex = index;
    int nextIndex = (firstIndex + 1) % 3;
    if (checkDrawable(firstIndex, nextIndex))
    {
        gs_out.worldPos = vec3(1.0f);
        gl_Position = gl_in[firstIndex].gl_Position;
        gl_PointSize = 5;
        EmitVertex();
        gl_Position = gl_in[nextIndex].gl_Position;
        gl_PointSize = 5;
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