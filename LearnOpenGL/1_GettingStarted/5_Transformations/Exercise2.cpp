#include <others/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <Shader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Utils.h>

static float vertices[] = {
    // positions           // texture coords
     0.5f,  0.5f, 0.0f,     1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,     1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,     0.0f, 1.0f    // top left 
};

static unsigned int indices[] = {
    0,1,3,
    2,3,1
};

static std::shared_ptr<Shader> shaderProgram;
static unsigned int VAO, VBO, EBO;

void exercise2_setup(GLFWwindow * window)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels, width1, height1, nrChannels1;
    std::string path;
    getProjectFilePath("Assets/container.jpg", path);
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    getProjectFilePath("Assets/awesomeface.png", path);
    unsigned char *data1 = stbi_load(path.c_str(), &width1, &height1, &nrChannels1, 0);

    unsigned int texture, texture1;
    glGenTextures(1, &texture);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
    stbi_image_free(data1);

    shaderProgram = std::make_shared<Shader>("Shaders/1_5/VertexShader15.vert", "Shaders/1_5/FragmentShader15.frag", nullptr);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    shaderProgram->use();
    shaderProgram->setInt("texture0", 0);
    shaderProgram->setInt("texture1", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
}

void exercise2_imgui(GLFWwindow * window)
{

}

int exercise2(GLFWwindow * window)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));

    shaderProgram->setMat4("transform", trans);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
    float scale = abs(sin((float)glfwGetTime()));
    trans = glm::scale(trans, glm::vec3(scale, scale, 1.0f));
    
    shaderProgram->setMat4("transform", trans);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    return 0;

}
