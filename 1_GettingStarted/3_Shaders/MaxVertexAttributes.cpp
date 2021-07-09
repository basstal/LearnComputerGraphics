#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// void processInput(GLFWwindow *window)
// {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     {
//         glfwSetWindowShouldClose(window, true);
//     }
// }
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
    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //
    //
    // glfwMakeContextCurrent(window);
    //
    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    // {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return -1;
    // }

    
    
    // while(!glfwWindowShouldClose(window))
    // {
        // processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);
        // glfwSwapBuffers(window);
        // glfwPollEvents();
    // }
    // glfwTerminate();
    return 0;
}