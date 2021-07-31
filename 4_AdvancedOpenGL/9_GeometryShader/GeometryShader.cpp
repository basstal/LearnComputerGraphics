/*
At the start of a geometry shader we need to declare the type of primitive input we're receiving from the vertex shader. We do this by declaring a layout specifier in front of the in keyword. This input layout qualifier can take any of the following primitive values:

points: when drawing GL_POINTS primitives (1).
lines: when drawing GL_LINES or GL_LINE_STRIP (2).
lines_adjacency: GL_LINES_ADJACENCY or GL_LINE_STRIP_ADJACENCY (4).
triangles: GL_TRIANGLES, GL_TRIANGLE_STRIP or GL_TRIANGLE_FAN (3).
triangles_adjacency : GL_TRIANGLES_ADJACENCY or GL_TRIANGLE_STRIP_ADJACENCY (6).

We also need to specify a primitive type that the geometry shader will output and we do this via a layout specifier in front of the out keyword. Like the input layout qualifier, the output layout qualifier can take several primitive values:

points
line_strip
triangle_strip

*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>

static unsigned int vao, vbo;

static float points[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

static std::shared_ptr<Shader> geometryShaderProgram;

void geometryShader_setup(GLFWwindow * window)
{
    geometryShaderProgram = std::make_shared<Shader>("Shaders/4_9/SimpleGeometryVS.vert", "Shaders/4_9/SimpleGeometryFS.frag", "Shaders/4_9/SimpleGeometryGS.geom");


    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void geometryShader_imgui(GLFWwindow * window)
{

}

int geometryShader(GLFWwindow * window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geometryShaderProgram->use();
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 4);

    return 0;
}
