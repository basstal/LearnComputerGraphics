#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>


float cubeVertices[] = {
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

// settings
int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void DrawCube(Shader shader, glm::vec3 position)
{
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);	// move top-left
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void frame_buffer_callback(GLFWwindow * window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
        glViewport(0, 0, width, height);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "AdvancedGLSL", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    
    Shader redShader("Shaders/4_8/UniformBufferObjectVS.vert", "Shaders/4_8/UniformBufferObjectFS1.frag", nullptr);
    Shader greenShader("Shaders/4_8/UniformBufferObjectVS.vert", "Shaders/4_8/UniformBufferObjectFS2.frag", nullptr);
    Shader blueShader("Shaders/4_8/UniformBufferObjectVS.vert", "Shaders/4_8/UniformBufferObjectFS3.frag", nullptr);
    Shader yellowShader("Shaders/4_8/UniformBufferObjectVS.vert", "Shaders/4_8/UniformBufferObjectFS4.frag", nullptr);

    unsigned int redUniformBlockIndex = glGetUniformBlockIndex(redShader.ID, "Matrices");
    unsigned int greenUniformBlockIndex = glGetUniformBlockIndex(greenShader.ID, "Matrices");
    unsigned int blueUniformBlockIndex = glGetUniformBlockIndex(blueShader.ID, "Matrices");
    unsigned int yellowUniformBlockIndex = glGetUniformBlockIndex(yellowShader.ID, "Matrices");
    glUniformBlockBinding(redShader.ID, redUniformBlockIndex, 0);
    glUniformBlockBinding(greenShader.ID, greenUniformBlockIndex, 0);
    glUniformBlockBinding(blueShader.ID, blueUniformBlockIndex, 0);
    glUniformBlockBinding(yellowShader.ID, yellowUniformBlockIndex, 0);

    unsigned int ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glm::mat4 view = camera.GetViewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glBindVertexArray(cubeVAO);
          
        DrawCube(redShader, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
        DrawCube(greenShader, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
        DrawCube(blueShader, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
        DrawCube(yellowShader, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}