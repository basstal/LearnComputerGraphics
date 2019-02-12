#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include <camera.h>
#include <shader.h>
#include <model.h>

#include <glm/matrix.hpp>

using namespace std;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = glfwGetTime();

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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow * window = glfwCreateWindow(800, 600, "chapter3", NULL, NULL);
    if (window == NULL)
    {
        cout << "ERROR::CREATE WINDOW FAILED!" << endl;
        glfwTerminate();
        return -1;
    }

    // 使用当前window作为current context
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "ERROR::GLAD LOAD FAILED!" << endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, window_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Model model = Model("/Users/wangjunke/Documents/OpenGL/OpenGL/nanosuit/nanosuit.mtl");
        Shader shader = Shader("VertexShader3.glsl", "FragmentShader3.glsl");

        glm::mat4 model1;
        glm::mat4 view = mainCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/600, 0.1f, 100.0f);
        glm::vec3 viewPos = mainCamera.Position;

        shader.setMat4("model", model1);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("viewPos", viewPos);
        shader.setFloat("material.shininess", 0.5f);

        shader.setVec3("dirLight.direction", glm::vec3(0.2f, 1.0f, 0.0f));
        shader.setVec3("dirLight.ambient", glm::vec3(0.5f));
        shader.setVec3("dirLight.diffuse", glm::vec3(0.3f));
        shader.setVec3("dirLight.specular", glm::vec3(0.3f));

        for ( int i = 0 ; i < 2; ++ i)
        {
            // shader.setVec3("pointLights[" + i + "].position", glm::vec3(0.2f, 1.0f, 0.0f));
            // shader.setVec3("pointLights[" + i + "].ambient", glm::vec3(0.5f));
            // shader.setVec3("pointLights[" + i + "].diffuse", glm::vec3(0.3f));
            // shader.setVec3("pointLights[" + i + "].specular", glm::vec3(0.3f));
        }

        model.Draw(shader);

        // 交换当前渲染帧和缓冲帧
        glfwSwapBuffers(window);
        // glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
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
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

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