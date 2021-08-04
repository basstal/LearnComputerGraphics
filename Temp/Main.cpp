#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <Camera.h>
#include <Model.h>

#include <glm/matrix.hpp>

#include <Utils.h>

using namespace std;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = (float)glfwGetTime();
int WIDTH = 1920;
int HEIGHT = 1080;
// bool allowCursorPosCallback = false;

Camera mainCamera = Camera(glm::vec3(0.0f, 9.0f, 7.0f));

void frame_buffer_size_callback(GLFWwindow *, int width, int height);
void window_pos_callback(GLFWwindow * window, double xPos, double yPos);
void scroll_callback(GLFWwindow *, double, double);

void processInput(GLFWwindow * );


static bool bCursorOff = false;
static bool bPressed;

static void switch_cursor(GLFWwindow * window)
{
    if (!bCursorOff)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, window_pos_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        firstMove = true;
    }
    bCursorOff = !bCursorOff;
}

Shader * shader = nullptr;
Shader * postprocessShader = nullptr;
Shader * gBufferShader = nullptr;
Shader * debugQuad = nullptr;
unsigned int framebuffer, frameTexture, renderbuffer, ppFramebuffer, ppFrameTexture;
float scaleFactor = 1.0f;

void recompileShaders()
{
    if (!shader)
    {
        shader = new Shader("Temp/ModelVS33.vert", "Temp/ModelFS33.frag", nullptr);
    }
    else
    {
        shader->recompileFromSource();
    }
    
    if (!postprocessShader)
    {
        postprocessShader = new Shader("Temp/PostProcessingSharpen.vert", "Temp/PostProcessingSharpen.frag", nullptr);
    }
    else
    {
        postprocessShader->recompileFromSource();
    }
    if (!gBufferShader)
    {
        gBufferShader = new Shader("Temp/GBufferVS.vert", "Temp/GBufferFS.frag", nullptr);
    }
    else
    {
        gBufferShader->recompileFromSource();
    }
    if (!debugQuad)
    {
        debugQuad = new Shader("Temp/DebugQuadVS.vert", "Temp/DebugQuadFS.frag", nullptr);
    }
    else
    {
        debugQuad->recompileFromSource();
    }
    
}

unsigned int gBufferFBO, gPosition, gNormal, gAlbedoSpec, depthRB;

void onWindowSizeChanged()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glDeleteTextures(1, &frameTexture);
    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);


    glDeleteRenderbuffers(1, &renderbuffer);
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "framebuffer incomplete!" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, ppFramebuffer);

    glDeleteTextures(1, &ppFrameTexture);
    glGenTextures(1, &ppFrameTexture);
    glBindTexture(GL_TEXTURE_2D, ppFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppFrameTexture, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "framebuffer incomplete!" << endl;
    }



    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
    

    // position color buffer
    glDeleteTextures(1,  &gPosition);
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal color buffer
    glDeleteTextures(1, &gNormal);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // color + specular color buffer
    glDeleteTextures(1, &gAlbedoSpec);
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
    glDrawBuffers(3, attachments);
    
    glDeleteRenderbuffers(1, &depthRB);
    glGenRenderbuffers(1, &depthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR::FRAME BUFFER INIT FAILED!" <<endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Model", NULL, NULL);
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
    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
    glfwSetScrollCallback(window, scroll_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    Model backpack = Model("Temp/Model/jdxc.obj", true);
    recompileShaders();
    

    float materialShininess = 32.0f;
    glm::vec3 pointLightPosition = glm::vec3(5.0, 5.0f, 5.0f);
    glm::vec3 clear_color = glm::vec3(0);
    float pointAmbientStrength = 0.2f;
    float pointDiffuseStrength = 0.6f;
    float pointSpecularStrength = 1.0f;

    float constant = 1.0f;
    float linear = 0.2f;
    float quadratic = 0.07f;

    glGenFramebuffers(1, &framebuffer);
    glGenFramebuffers(1, &gBufferFBO);
    glGenFramebuffers(1, &ppFramebuffer);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    float vertices[30] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    onWindowSizeChanged();


    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Editor");
        {
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

            ImGui::Separator();
            if (bCursorOff)
            {
                ImGui::Text("Press P to release control of the camera, and show cursor.");
            }
            else
            {
                ImGui::Text("Press P or Button follows to take control of the camera");
                if(ImGui::Button("Posses camera") && !bCursorOff)
                {
                    switch_cursor(window);
                }
            }
            if (ImGui::Button("Recompile Shaders"))
            {
                recompileShaders();
            }
            ImGui::SliderFloat("Scale Factor", &scaleFactor, 0.1, 100.0f);
            ImGui::Separator();
            ImGui::SliderFloat("Camera Speed", (float *)&mainCamera.MovementSpeed, .0f, 5.0f);
            ImGui::SliderFloat("Camera Sensitivity", (float *)&mainCamera.MouseSensitivity, .0f, .3f);

            glm::vec3 pos = mainCamera.Position;
            ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
            ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", mainCamera.Yaw, mainCamera.Pitch);
            ImGui::Separator();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(scaleFactor));
        
        glm::mat4 view = mainCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)WIDTH/HEIGHT, 0.1f, 100.0f);

        shader->use();

        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        shader->setVec3("viewPos", mainCamera.Position);
        shader->setFloat("material.shininess", materialShininess);

        shader->setVec3("pointLight.position", pointLightPosition);
        shader->setVec3("pointLight.ambient", glm::vec3(pointAmbientStrength));
        shader->setVec3("pointLight.diffuse", glm::vec3(pointDiffuseStrength));
        shader->setVec3("pointLight.specular", glm::vec3(pointSpecularStrength));

        shader->setFloat("pointLight.constant", constant);
        shader->setFloat("pointLight.linear", linear);
        shader->setFloat("pointLight.quadratic", quadratic);

        backpack.Draw(*shader);

        glBindFramebuffer(GL_FRAMEBUFFER, ppFramebuffer);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameTexture);
        postprocessShader->use();
        // postprocessShader->setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gBufferShader->use();
        gBufferShader->setMat4("model", model);
        gBufferShader->setMat4("view", view);
        gBufferShader->setMat4("projection", projection);
        
        backpack.Draw(*gBufferShader);



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        debugQuad->use();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5));
        glm::mat4 bottomLeft = glm::translate(model, glm::vec3(-1, -1, 0.0f));
        debugQuad->setMat4("model", bottomLeft);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glm::mat4 topRight = glm::translate(model, glm::vec3(1, 1, 0.0f));
        debugQuad->setMat4("model", topRight);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glm::mat4 topLeft = glm::translate(model, glm::vec3(-1, 1, 0.0f));
        debugQuad->setMat4("model", topLeft);
        glBindTexture(GL_TEXTURE_2D, ppFrameTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
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
    if (width > 0 && height > 0)
    {
        WIDTH = width;
        HEIGHT = height;
        onWindowSizeChanged();
        glViewport(0, 0, width, height);
    }
}

void window_pos_callback(GLFWwindow * window, double xPos, double yPos)
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

void scroll_callback(GLFWwindow * window, double offsetX, double offsetY)
{
    mainCamera.ProcessMouseScroll((float)offsetY);
}

void processInput(GLFWwindow * window)
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && bPressed)
    {
        bPressed = false;
        switch_cursor(window);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}