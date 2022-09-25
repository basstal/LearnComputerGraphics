/*

Blending in OpenGL happens with the following equation:

C¯result=C¯source∗Fsource+C¯destination∗Fdestination

C¯source: the source color vector. This is the color output of the fragment shader.
C¯destination: the destination color vector. This is the color vector that is currently stored in the color buffer.
Fsource: the source factor value. Sets the impact of the alpha value on the source color.
Fdestination: the destination factor value. Sets the impact of the alpha value on the destination color.

*/
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>

#include <iostream>
#include <vector>
#include <map>

#include "Utils.h"

static bool wireframe = false;

static float cubeVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    // Right face
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
};

static float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

static float windowVertices[] = {
    -0.5f,  -0.5f,  0.0f,   1.0f,   1.0f,
    -0.5f,  0.5f,   0.0f,   1.0f,   0.0f,
    0.5f,   -0.5f,  0.0f,   0.0f,   1.0f,
    0.5f,   -0.5f,  0.0f,   0.0f,   1.0f,
    -0.5f,  0.5f,   0.0f,   1.0f,   0.0f,
    0.5f,   0.5f,   0.0f,   0.0f,   0.0f,
};

static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow *window);

// settings
extern int WIDTH, HEIGHT;

// camera
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float lastX = (float)WIDTH  / 2.0;
static float lastY = (float)HEIGHT / 2.0;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;
static float lastTime = 0.0f;

static unsigned int cubeVAO, cubeVBO;
static unsigned int vegetationVAO, vegetationVBO;
static unsigned int planeVAO, planeVBO;
static unsigned int cubeTexture, floorTexture, grassTexture;

static std::shared_ptr<Shader> simpleShader;
static std::shared_ptr<Shader> vegetationShader;
static std::vector<glm::vec3> vegetation;


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
        firstMouse = true;
    }
    bCursorOff = !bCursorOff;
}

void blending_setup(GLFWwindow *window)
{
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    cubeTexture  = loadImage("Assets/marble.jpg", false);
    floorTexture = loadImage("Assets/metal.png", false);
    grassTexture = loadImage("Assets/grass.png", false);

    simpleShader = std::make_shared<Shader>("Shaders/4_1/VertexShader.vert", "Shaders/4_1/FragmentShader.frag", nullptr);
    vegetationShader = std::make_shared<Shader>("Shaders/4_1/VertexShader.vert", "Shaders/4_3/VegetationFragmentShader.frag", nullptr);

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vegetation.clear();
    vegetation.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    vegetation.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    vegetation.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    vegetation.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));  

    // vegetation VAO
    glGenVertexArrays(1, &vegetationVAO);
    glGenBuffers(1, &vegetationVBO);
    glBindVertexArray(vegetationVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vegetationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    if (!wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void blending_imgui(GLFWwindow * window)
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
    glm::vec3 pos = camera.Position;
    ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
    ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", camera.Yaw, camera.Pitch);

}

int blending(GLFWwindow *window)
{
    processInput(window);
        
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleShader->use();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    simpleShader->setMat4("view", view);
    simpleShader->setMat4("projection", projection);

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(1.0));
    simpleShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0));
    simpleShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    vegetationShader->use();
    vegetationShader->setMat4("projection", projection);
    vegetationShader->setMat4("view", view);
    glBindVertexArray(vegetationVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    // ** clear the top edge's line
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for(unsigned int i = 0; i < vegetation.size(); i++) 
    {
        model = glm::mat4(1.0f);
        glm::vec3 vegetationPos = vegetation[i];
        model = glm::translate(model, vegetationPos);
        vegetationShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
    return 0;

}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow *window)
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}