// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


static float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

static int indices[] = {
    0, 1, 3,
    1, 2, 3
};

static float mixParam = 0.5f;
// void processInput(GLFWwindow *window)
// {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     {
//         glfwSetWindowShouldClose(window, true);
//     }
// }
static unsigned int texture, texture1;
static Shader * shaderProgram;
static unsigned int VAO, EBO, VBO;

void exercise1_setup(GLFWwindow * window)
{
    // glfwInit();
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
    // if (window == NULL)
    // {
    //     std::cout << "Failed to create GLFW window" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }
    // glfwMakeContextCurrent(window);

    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    // {
    //     std::cout << "Failed to initialize GLAD" << std::endl;
    //     return -1;
    // }

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../../Assets/awesomeface.png", &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<< "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    data = stbi_load("../../Assets/container.jpg", &width, &height, &nrChannels, 0);
    // unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    
    
    shaderProgram = new Shader("../../Shaders/1_4/VertexShader14.vs", "../../Shaders/1_4/FragmentShader14.fs", NULL);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ** for debug use
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    shaderProgram->use();
    shaderProgram->setInt("texture0", 0);
    shaderProgram->setInt("texture1", 1);

    // while(!glfwWindowShouldClose(window))
    // {
    //     processInput(window);

        

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();    
    // }
    // glfwTerminate();
    // return 0;
}

void exercise1_imgui(GLFWwindow * window)
{
    // std::cout<< "111" << std::endl;
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Params"))
    {
        ImGui::SliderFloat("mixParam", (float*)&mixParam, 0.0f, 1.0f);
    }
    // ImGui::SetWindowSize(ImVec2(345, 200));
}
int exercise1(GLFWwindow * window)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixParam += 0.01f;
    }
    else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixParam -= 0.01f;
    }
    shaderProgram->setFloat("mixParam", mixParam);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    return 0;
}