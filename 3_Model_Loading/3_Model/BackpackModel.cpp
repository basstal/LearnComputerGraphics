#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <Camera.h>
#include <model.h>

#include <glm/matrix.hpp>

using namespace std;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = (float)glfwGetTime();
float screenWidth = 1920;
float screenHeight = 1080;
bool allowCursorPosCallback = false;

Camera mainCamera = Camera();

void frame_buffer_size_callback(GLFWwindow *, int width, int height);
void window_pos_callback(GLFWwindow * window, double xPos, double yPos);
void scroll_callback(GLFWwindow *, double, double);

void processInput(GLFWwindow * );

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(screenWidth, screenHeight, "Model-backpack", NULL, NULL);
    if (window == NULL)
    {
        cout << "ERROR::CREATE WINDOW FAILED!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "ERROR::GLAD LOAD FAILED!" << endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, window_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_DEPTH_TEST);

    Model backpack = Model("Src/resources/backpack/backpack.obj", true);
    Shader shader = Shader("../../Shaders/3_3/ModelVS33.vs", "../../Shaders/3_3/ModelFS33.fs", NULL);
    float materialShininess = 32.0f;
    glm::vec3 pointLightPosition = glm::vec3(1.2f, 1.0f, 1.0f);
    glm::vec3 clear_color = glm::vec3(0);
    float pointAmbientStrength = 0.2f;
    float pointDiffuseStrength = 0.6f;
    float pointSpecularStrength = 1.0f;

    float constant = 1.0f;
    float linear = 0.2f;
    float quadratic = 0.07f;
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Editor");
        ImGui::InputFloat("Material shininess", &materialShininess);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        ImGui::Spacing();
        ImGui::Text("Lights Properties");
        ImGui::Spacing();
        ImGui::SliderFloat("pointAmbientStrength", &pointAmbientStrength, 0, 1);
        ImGui::SliderFloat("pointDiffuseStrength", &pointDiffuseStrength, 0, 1);
        ImGui::SliderFloat("pointSpecularStrength", &pointSpecularStrength, 0, 1);

        ImGui::Spacing();
        ImGui::SliderFloat("constant", &constant, 0, 1);
        ImGui::SliderFloat("linear", &linear, 0, 1);
        ImGui::SliderFloat("quadratic", &quadratic, 0, 1);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // Rendering
        ImGui::Render();


        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f));
        
        glm::mat4 view = mainCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)screenWidth/screenHeight, 0.1f, 100.0f);

        shader.use();

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setVec3("viewPos", mainCamera.Position);
        shader.setFloat("material.shininess", materialShininess);

        shader.setVec3("pointLight.position", pointLightPosition);
        shader.setVec3("pointLight.ambient", glm::vec3(pointAmbientStrength));
        shader.setVec3("pointLight.diffuse", glm::vec3(pointDiffuseStrength));
        shader.setVec3("pointLight.specular", glm::vec3(pointSpecularStrength));

        shader.setFloat("pointLight.constant", constant);
        shader.setFloat("pointLight.linear", linear);
        shader.setFloat("pointLight.quadratic", quadratic);

        backpack.Draw(shader);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void window_pos_callback(GLFWwindow * window, double xPos, double yPos)
{
    if (allowCursorPosCallback)
    {
        if ( firstMove )
        {
            lastX = (float)xPos;
            lastY = (float)yPos;
            firstMove = false;
        }

        float offsetX = (float)xPos - lastX;
        float offsetY = lastY - (float)yPos;
        lastX = (float)xPos;
        lastY = (float)yPos;

        mainCamera.ProcessMouseMovement(offsetX, offsetY);
    }
}

void scroll_callback(GLFWwindow * window, double offsetX, double offsetY)
{
    mainCamera.ProcessMouseScroll((float)offsetY);
}

void processInput(GLFWwindow * window)
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        allowCursorPosCallback = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        firstMove = true;
        allowCursorPosCallback = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (allowCursorPosCallback)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mainCamera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mainCamera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mainCamera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}