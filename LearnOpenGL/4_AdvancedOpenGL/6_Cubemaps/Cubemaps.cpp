#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <others/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <Camera.h>
#include <Utils.h>


#include <iostream>
#include <vector>
#include <map>

static bool wireframe = false;

static float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

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
static float lastFrame = 0.0f;


static unsigned int cubeVAO, cubeVBO;
static unsigned int cubeTexture = 0;
static unsigned int skyboxVAO, skyboxVBO;
static unsigned int skyboxTextures = 0;

static std::shared_ptr<Shader> shader;
static std::shared_ptr<Shader> simpleShader;

static std::vector<std::string> skyboxTexs = {
    "Assets/skybox/right.jpg",
    "Assets/skybox/left.jpg",
    "Assets/skybox/top.jpg",
    "Assets/skybox/bottom.jpg",
    "Assets/skybox/front.jpg",
    "Assets/skybox/back.jpg",
};

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

void cubemaps_setup(GLFWwindow * window)
{
    glfwSetScrollCallback(window, scroll_callback);

    // build and compile shaders
    // -------------------------
    shader = std::make_shared<Shader>("Shaders/4_6/CubemapsVS.vert", "Shaders/4_6/CubemapsFS.frag", nullptr);
    simpleShader = std::make_shared<Shader>("Shaders/4_1/VertexShader.vert", "Shaders/4_1/FragmentShader.frag", nullptr);

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Skybox vao
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    if (cubeTexture == 0)
    {
        cubeTexture  = loadImage("Assets/container.jpg", false);
    }
    if (skyboxTextures == 0)
    {
        skyboxTextures = LoadSkyboxTex(skyboxTexs);
    }

    if (!wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void cubemaps_imgui(GLFWwindow * window)
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


int cubemaps(GLFWwindow * window)
{
    processInput(window);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    simpleShader->use();
    simpleShader->setMat4("projection", projection);
    simpleShader->setMat4("view", camera.GetViewMatrix());
    simpleShader->setMat4("model", glm::mat4(1.0));

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader->use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setInt("cubeTexture", 0);

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextures);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    return 0;

}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow *window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

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