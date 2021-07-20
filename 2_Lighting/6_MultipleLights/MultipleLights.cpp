
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <Shader.h>
#include <Camera.h>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// #define STB_IMAGE_IMPLEMENTATION
#include <others/stb_image.h>
#include <Utils.h>

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  2.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

glm::vec3 pointLightRepresentColor[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.2f, 1.0f),
    glm::vec3(0.5f, 0.2f, 1.0f),
};

const int WIDTH = 1920;
const int HEIGHT = 1080;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = (float)glfwGetTime();
bool moveMouse = true;

void frame_buffer_callback(GLFWwindow * window, int , int );
void scroll_callback(GLFWwindow *, double , double);
void mouse_callback(GLFWwindow * window, double xPos, double yPos);
void processInput(GLFWwindow *);
unsigned int loadImage(const char * fileName, GLint format, bool, GLint);

static bool bCursorOff = false;
static bool bPressed;

static void switch_cursor(GLFWwindow * window)
{
    if (!bCursorOff)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        moveMouse = true;
    }
    bCursorOff = !bCursorOff;
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "MultipleLights", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR::CREATWINDOW::FAILED!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
    {
        std::cout << "ERROR::LOAD GL LOADER::FAILED!" << std::endl;
        return -1;
    }

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

    // glfwSetFramebufferSizeCallback(window, frame_buffer_callback);

    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    unsigned int diffuseTexture, specularTexture;
    std::string path;
    getProjectFilePath("Assets/diffuse_map.png", path);
    diffuseTexture = loadImage(path.c_str(), GL_RGBA, GL_FALSE, GL_REPEAT);
    getProjectFilePath("Assets/specular_map.png", path);
    specularTexture = loadImage(path.c_str(), GL_RGBA, GL_FALSE, GL_REPEAT);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ** cube of light source
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);


    std::string vsPath, fsPath;
    getProjectFilePath("Shaders/2_6/MultipleLightsVS26.vs", vsPath);
    getProjectFilePath("Shaders/2_6/MultipleLightsFS26.fs", fsPath);
    Shader shaderProgram = Shader(vsPath.c_str(), fsPath.c_str(), nullptr);
    getProjectFilePath("Shaders/2_2/VertexShader22.vs", vsPath);
    getProjectFilePath("Shaders/2_6/LightFS26.fs", fsPath);
    Shader lampShader = Shader(vsPath.c_str(), fsPath.c_str(), nullptr);

    glEnable(GL_DEPTH_TEST);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float cutOff = 12.0f;
    float outerCutOff = 17.5f;
    float spotAmbientStrength = 0.2f;
    float spotDiffuseStrength = 0.7f;
    float spotSpecularStrength = 1.0f;

    float pointConstant = 1.0f;
    float pointLinear = 0.07f;
    float pointQuadratic = 0.17f;
    
    float pointAmbientStrength = 0.0f;
    float pointDiffuseStrength = 0.4f;
    float pointSpecularStrength = 1.0f;

    float dirAmbientStrength = 0.2f;
    float dirDiffuseStrength = 0.4f;
    float dirSpecularStrength = 1.0f;

    glm::vec3 dirDirection = glm::vec3(0.0f, 1.0f, 0.2f);
    float materialShininess = 32.0f;

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ImGui::Begin("Editor"))
        {
            ImGui::InputFloat("Material shininess", &materialShininess);
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
            ImGui::Text("Lights Color");
            for (int i = 0; i < 4; ++i)
            {
                ImGui::ColorEdit3(("light" + std::to_string(i)).c_str(), glm::value_ptr(pointLightRepresentColor[i]));
            }
            ImGui::Text("Lights Properties");
            ImGui::Spacing();
            ImGui::SliderFloat3("dirDirection", glm::value_ptr(dirDirection), -5, 5);
            ImGui::SliderFloat("dirAmbientStrength", &dirAmbientStrength, 0, 1);
            ImGui::SliderFloat("dirDiffuseStrength", &dirDiffuseStrength, 0, 1);
            ImGui::SliderFloat("dirSpecularStrength", &dirSpecularStrength, 0, 1);

            ImGui::Spacing();
            ImGui::SliderFloat("pointAmbientStrength", &pointAmbientStrength, 0, 1);
            ImGui::SliderFloat("pointDiffuseStrength", &pointDiffuseStrength, 0, 1);
            ImGui::SliderFloat("pointSpecularStrength", &pointSpecularStrength, 0, 1);

            ImGui::Spacing();
            ImGui::SliderFloat("cutOff", &cutOff, 10.0f, 45.0f);
            ImGui::SliderFloat("outerCutOff", &outerCutOff, cutOff, cutOff + 20.0f);
            outerCutOff = glm::max(outerCutOff, cutOff);
            ImGui::SliderFloat("spotAmbientStrength", &spotAmbientStrength, 0, 1);
            ImGui::SliderFloat("spotDiffuseStrength", &spotDiffuseStrength, 0, 1);
            ImGui::SliderFloat("spotSpecularStrength", &spotSpecularStrength, 0, 1);

            
            ImGui::End();
        }
        
        if(ImGui::Begin("Viewer"))
        {
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
            ImGui::Separator();

            glm::vec3 pos = camera.Position;
            ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
            ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", camera.Yaw, camera.Pitch);
            ImGui::Separator();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH/HEIGHT, 0.01f, 100.0f);
    
        lampShader.use();
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; ++ i)
        {
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lampShader.setVec3("color", pointLightRepresentColor[i]);
            lampShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        shaderProgram.use();
        shaderProgram.setMat4("view", view);
        shaderProgram.setMat4("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularTexture);

        shaderProgram.setInt("material.diffuse", 0);
        shaderProgram.setInt("material.specular", 1);
        shaderProgram.setFloat("material.shininess", materialShininess);

        shaderProgram.setVec3("viewPos", camera.Position);

        shaderProgram.setVec3("dirLight.direction", dirDirection);
        shaderProgram.setVec3("dirLight.ambient", glm::vec3(dirAmbientStrength));
        shaderProgram.setVec3("dirLight.diffuse", glm::vec3(dirDiffuseStrength));
        shaderProgram.setVec3("dirLight.specular", glm::vec3(dirSpecularStrength));

        for ( int i = 0; i < 4; ++ i)
        {
            std::string prefix = "pointLights[" + std::to_string(i) + "]";
            shaderProgram.setVec3(prefix + ".position", pointLightPositions[i]);
            shaderProgram.setFloat(prefix + ".constant", pointConstant);
            shaderProgram.setFloat(prefix + ".linear", pointLinear);
            shaderProgram.setFloat(prefix + ".quadratic", pointQuadratic);

            shaderProgram.setVec3(prefix + ".ambient", glm::vec3(pointAmbientStrength) * pointLightRepresentColor[i]);
            shaderProgram.setVec3(prefix + ".diffuse", glm::vec3(pointDiffuseStrength) * pointLightRepresentColor[i]);
            shaderProgram.setVec3(prefix + ".specular", glm::vec3(pointSpecularStrength) * pointLightRepresentColor[i]);
        }
        shaderProgram.setVec3("spotLight.position", camera.Position);
        shaderProgram.setVec3("spotLight.direction", camera.Front);
        shaderProgram.setFloat("spotLight.cutOff", glm::cos(glm::radians(cutOff)));
        shaderProgram.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
        shaderProgram.setVec3("spotLight.ambient", glm::vec3(spotAmbientStrength));
        shaderProgram.setVec3("spotLight.diffuse", glm::vec3(spotDiffuseStrength));
        shaderProgram.setVec3("spotLight.specular", glm::vec3(spotSpecularStrength));

        glBindVertexArray(VAO);

        for ( int i = 0; i < 10 ; ++ i)
        {

            glm::mat4 model = glm::mat4(1.0);
            model = glm::rotate(model, glm::radians(30.0f) * i, glm::vec3(0.3f, 0.7f, 0.0f));
            model = glm::translate(model, cubePositions[i]);

            shaderProgram.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        
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

void frame_buffer_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && bPressed)
    {
        bPressed = false;
        switch_cursor(window);
    }
}

void scroll_callback(GLFWwindow *window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

void mouse_callback(GLFWwindow * window, double xPos, double yPos)
{
    if (moveMouse)
    {
        lastX = (float)xPos;
        lastY = (float)yPos;
        moveMouse = false;
    }

    float offsetX = (float)xPos - lastX;
    float offsetY = lastY - (float)yPos;
    lastX = (float) xPos;
    lastY = (float) yPos;

    camera.ProcessMouseMovement(offsetX, offsetY);
}

unsigned int loadImage(const char * fileName, GLint format, bool verticalFlip, GLint wrapMode)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(verticalFlip);
    int width, height, nrchannel;
    unsigned char * data = stbi_load(fileName, &width, &height, &nrchannel, 0);
    if ( data != NULL )
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR::LOAD IMAGE::FAILED!" << std::endl;
    }
    
    stbi_image_free(data);
    return texture;
}