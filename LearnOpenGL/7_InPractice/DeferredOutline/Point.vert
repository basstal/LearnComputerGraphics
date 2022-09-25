#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in float legalPoints;
layout (location = 2) in float removeEdgeToPointA;
layout (location = 3) in float removeEdgeToPointB;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT{
    float legalPoints;
    vec3 worldPos;
    bool removeEdgeToPointA;
    bool removeEdgeToPointB;
    vec3 vertex;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    vs_out.legalPoints = legalPoints;
    vs_out.removeEdgeToPointA = removeEdgeToPointA > 0;
    vs_out.removeEdgeToPointB = removeEdgeToPointB > 0;
    vs_out.vertex = vertex;
    vs_out.worldPos = vec3(model * vec4(vertex, 1.0));
}