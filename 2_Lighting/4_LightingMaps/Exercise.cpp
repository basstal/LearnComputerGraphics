#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
#include <stb_image.h>

static float vertices[] = {
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

static glm::vec3 cubePositions[] = {
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

static glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  2.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

static glm::vec3 pointLightRepresentColor[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.2f, 1.0f),
    glm::vec3(0.5f, 0.2f, 1.0f),
};

static const int WIDTH = 1920;
static const int HEIGHT = 1080;

static Camera camera = Camera(glm::vec3(-2.0, -0.5, 2.5), glm::vec3(0, 1, 0), -35.0f, 14.0f);
static float lastX = 0.0f;
static float lastY = 0.0f;
static float lastTime = (float)glfwGetTime();
static bool moveMouse = true;

static void frame_buffer_callback(GLFWwindow * window, int , int );
static void scroll_callback(GLFWwindow *, double , double);
static void mouse_callback(GLFWwindow * window, double xPos, double yPos);
static void processInput(GLFWwindow *);

static glm::vec3 lightPos;
static glm::mat4 model;
static unsigned int lightVAO;
static unsigned int VAO, VBO;
static std::shared_ptr<Shader> shaderProgram_exercise2;
static std::shared_ptr<Shader> shaderProgram;
static std::shared_ptr<Shader> lampShader;
static unsigned int diffuseMap, specularMap, emissionMap, specularMap_exercise3;
static unsigned int exercise_index = 0;


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

void exercise_setup(GLFWwindow * window)
{
    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    // GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Chapter2", NULL, NULL);
    // if (window == NULL)
    // {
    //     std::cout << "ERROR::CREATWINDOW::FAILED!" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }

    // glfwMakeContextCurrent(window);

    // if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
    // {
    //     std::cout << "ERROR::LOAD GL LOADER::FAILED!" << std::endl;
    //     return -1;
    // }

    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);

    glfwSetScrollCallback(window, scroll_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);

    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    int width, height, nrChannels, width1, height1, nrChannels1, width2, height2, nrChannels2, width3, height3, nrChannels3;
    unsigned char *data = stbi_load("../../Assets/diffuse_map.png", &width, &height, &nrChannels, 0);
    unsigned char *data1 = stbi_load("../../Assets/specular_map.png", &width1, &height1, &nrChannels1, 0);
    // ** Exercise 4
    unsigned char *dataEmission = stbi_load("../../Assets/matrix.jpg", &width2, &height2, &nrChannels2, 0);
    // ** Exercise 3
    unsigned char *data_exercise3 = stbi_load("../../Assets/specular_color_map.png", &width3, &height3, &nrChannels3, 0);

    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    glGenTextures(1, &emissionMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    // ** Exercise 3
    glBindTexture(GL_TEXTURE_2D, specularMap_exercise3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data_exercise3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_exercise3);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    // ** Exercise 4
    glBindTexture(GL_TEXTURE_2D, emissionMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(dataEmission)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, dataEmission);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    stbi_image_free(dataEmission);
    stbi_image_free(data);
    stbi_image_free(data1);

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
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // ** Exercise 2
    shaderProgram_exercise2 = std::make_shared<Shader>("../../Shaders/2_4/SpecularMapVS24.vs", "../../Shaders/2_4/InvertingSpecularMapFS.fs", nullptr);
    shaderProgram = std::make_shared<Shader>("../../Shaders/2_4/SpecularMapVS24.vs", "../../Shaders/2_4/EmissionMapFS24.fs", nullptr);
    lampShader = std::make_shared<Shader>("../../Shaders/2_2/VertexShader22.vs", "../../Shaders/2_1/LightFragmentShader.fs", nullptr);

    lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    model = glm::mat4(1.0);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2));

    glEnable(GL_DEPTH_TEST);

    // while(!glfwWindowShouldClose(window))
    // {
        


    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }
    // glfwTerminate();
}

int exercise(GLFWwindow * window)
{
    processInput(window);
        
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH/HEIGHT, 0.01f, 100.0f);

    lampShader->use();
    lampShader->setMat4("model", model);
    lampShader->setMat4("view", view);
    lampShader->setMat4("projection", projection);
    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shaderProgram->use();
    shaderProgram->setMat4("model", glm::mat4(1.0));
    shaderProgram->setMat4("view", view);
    shaderProgram->setMat4("projection", projection);
    shaderProgram->setVec3("lightPos", lightPos);
    shaderProgram->setVec3("viewPos", camera.Position);
    shaderProgram->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    
    glm::vec3 lightColor = glm::vec3(1.0);

    // ** Exercise 1
    if (exercise_index == 0)
    {
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
    }

    shaderProgram->setVec3("light.ambient", lightColor * glm::vec3(0.5));
    shaderProgram->setVec3("light.diffuse", lightColor * glm::vec3(0.1));
    shaderProgram->setVec3("light.specular", lightColor); 
    shaderProgram->setFloat("material.shininess", 32.0f);
    shaderProgram->setInt("material.diffuse", 0);
    shaderProgram->setInt("material.specular", 1);

    // ** Exercise 4
    shaderProgram->setInt("material.emission", 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    if (exercise_index == 2)
    {
        glBindTexture(GL_TEXTURE_2D, specularMap_exercise3);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, specularMap);
    }

    // ** Exercise 4
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissionMap);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    return 0;

}


void exercise_imgui(GLFWwindow * window)
{
    ImGui::Separator();
    if (bCursorOff)
    {
        ImGui::Text("Press P to release control of the camera, and show cursor.");
    }
    else
    {
        ImGui::Text("Press P or belowd Button to take control of the camera");
        if(ImGui::Button("Posses camera") && !bCursorOff)
        {
            switch_cursor(window);
        }
    }
    glm::vec3 pos = camera.Position;
    ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
    ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", camera.Yaw, camera.Pitch);
}

static void frame_buffer_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow * window)
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

static void scroll_callback(GLFWwindow *window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

static void mouse_callback(GLFWwindow * window, double xPos, double yPos)
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