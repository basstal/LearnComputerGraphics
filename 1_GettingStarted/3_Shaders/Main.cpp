#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <FuncSet.h>

static void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

extern int maxVertexAttributes(GLFWwindow *);
extern int moreAttributes(GLFWwindow *);
extern int sendColor(GLFWwindow *);
extern int uniform(GLFWwindow *window);
extern int exercise(GLFWwindow * window);

extern void maxVertexAttributes_setup(GLFWwindow *);
extern void moreAttributes_setup(GLFWwindow *);
extern void sendColor_setup(GLFWwindow *);
extern void uniform_setup(GLFWwindow *);
extern void exercise_setup(GLFWwindow *);

extern void maxVertexAttributes_imgui(GLFWwindow *);

std::map<std::string, FuncSet> maps{
    {"maxVertexAttributes", FuncSet(maxVertexAttributes_setup, maxVertexAttributes, maxVertexAttributes_imgui)},
    {"moreAttributes", FuncSet(moreAttributes_setup, moreAttributes)},
    {"sendColor", FuncSet(sendColor_setup, sendColor)},
    {"uniform", FuncSet(uniform_setup, uniform)},
    {"exercise", FuncSet(exercise_setup, exercise)},
};

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Shaders", NULL, NULL);
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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    int (*current_draw)(GLFWwindow *) = nullptr;
    void (*current_imgui)(GLFWwindow *) = nullptr;

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Draw Functions");                          // Create a window called "Hello, world!" and append into it.
            for(auto entry : maps)
            {
                if (ImGui::Button(entry.first.c_str()))
                {
                    if(entry.second.setup)
                    {
                        entry.second.setup(window);
                    }
                    current_draw = entry.second.draw;
                    current_imgui = entry.second.imgui;
                }
            }
            if (current_imgui)
            {
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