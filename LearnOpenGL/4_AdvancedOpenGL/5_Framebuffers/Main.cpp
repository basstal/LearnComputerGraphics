#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include "FuncSet.h"

static void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

extern int framebuffers(GLFWwindow *);
extern int exercise1(GLFWwindow * window);
extern int exercise2(GLFWwindow * window);

extern void framebuffers_setup(GLFWwindow *);
extern void exercise1_setup(GLFWwindow *);
extern void exercise2_setup(GLFWwindow *);

extern void framebuffers_imgui(GLFWwindow *);
extern void exercise1_imgui(GLFWwindow * );
extern void exercise2_imgui(GLFWwindow * );

extern void framebuffers_viewport(GLFWwindow *);
extern void exercise1_viewport(GLFWwindow *);
extern void exercise2_viewport(GLFWwindow *);

std::map<std::string, FuncSet> maps{
    {"framebuffers", FuncSet(framebuffers_setup, framebuffers, framebuffers_imgui, framebuffers_viewport)},
    {"exercise1", FuncSet(exercise1_setup, exercise1, exercise1_imgui, exercise1_viewport)},
    {"exercise2", FuncSet(exercise2_setup, exercise2, exercise2_imgui, exercise2_viewport)},
};

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void (*current_viewport)(GLFWwindow *) = nullptr;

int WIDTH = 1920, HEIGHT = 1080;
static void frame_buffer_callback(GLFWwindow * window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        WIDTH = width;
        HEIGHT = height;
        if (current_viewport)
        {
            current_viewport(window);
        }
        glViewport(0, 0, width, height);
    }
}
int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit())
    {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Framebuffers", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    auto firstEntry = maps.begin();
    FuncSet funcSet = firstEntry->second;
    int (*current_draw)(GLFWwindow *) = funcSet.draw;
    void (*current_imgui)(GLFWwindow *) = funcSet.imgui;
    current_viewport = funcSet.viewport;
    if (funcSet.setup)
    {
        funcSet.setup(window);
    }

    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
    if (current_viewport)
    {
        current_viewport(window);
    }

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Editor");                          
            for(auto entry : maps)
            {
                if (ImGui::Button(entry.first.c_str()))
                {
                    FuncSet funcSet = entry.second;
                    if(funcSet.setup)
                    {
                        funcSet.setup(window);
                    }
                    current_draw = funcSet.draw;
                    current_imgui = funcSet.imgui;
                    current_viewport = funcSet.viewport;
                    if (current_viewport)
                    {
                        current_viewport(window);
                    }
                }
            }
            if (current_imgui)
            {
                // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
                // Most ImGui functions would normally just crash if the context is missing.
                IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
                current_imgui(window);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (current_draw)
        {
            current_draw(window);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}