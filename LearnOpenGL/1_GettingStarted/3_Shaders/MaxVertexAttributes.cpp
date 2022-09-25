#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static int nrAttributes = 0;
void maxVertexAttributes_setup(GLFWwindow * window)
{
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
}

void maxVertexAttributes_imgui(GLFWwindow * window)
{
    
    ImGui::Text("Maximum nr of vertex attributes supported: %d", nrAttributes);
}

int maxVertexAttributes(GLFWwindow * window)
{
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}