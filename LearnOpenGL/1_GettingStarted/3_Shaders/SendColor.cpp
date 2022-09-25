#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos; // the position variable has attribute position 0\n"
"out vec4 vertexColor; // specify a color output to the fragment shader\n"
"void main()\n"
"{\n"
    "gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor\n"
    "vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color\n"
"}\n";

static const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  \n"
"void main()\n"
"{\n"
    "FragColor = vertexColor;\n"
"} \n";

static float vertices[] = {
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

static unsigned int vertexShader, fragmentShader;
static unsigned int shaderProgram;
static unsigned int VAO, VBO;


void sendColor_setup(GLFWwindow * window)
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

int sendColor(GLFWwindow * window)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);        
    return 0;
}