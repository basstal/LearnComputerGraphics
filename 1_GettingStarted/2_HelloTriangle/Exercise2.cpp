#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

/*
OpenGL doesn't simply transform all your 3D coordinates to 2D pixels on your screen; OpenGL only processes 3D coordinates when they're in a specific range between -1.0 and 1.0 on all 3 axes (x, y and z). All coordinates within this so called normalized device coordinates range will end up visible on your screen (and all coordinates outside this region won't).
*/
static float vertices[] = {
     0.0f,  -0.5f, 0.0f,
     1.0f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    
};

static float vertices1[] = {
    -1.0f, -0.5f, 0.0f,
     0.0f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
};

static const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n";

static const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n";

// ** 开启线框模式
static bool wireframe = false;

static int key_state_left_control = GLFW_RELEASE;
static int key_state_f = GLFW_RELEASE;

static int SCR_WIDTH = 1920;
static int SCR_HEIGHT = 1080;
static unsigned int shaderProgram;
static unsigned int VAO, VBO;
static unsigned int VAO1, VBO1;


static void processInputWireframe(GLFWwindow * window)
{
    // ** Ctrl+F 开启/关闭 线框模式
    int key_state_left_control_changed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    if (key_state_left_control_changed != key_state_left_control)
    {
        key_state_left_control = key_state_left_control_changed;
    }
    int key_state_f_changed = glfwGetKey(window, GLFW_KEY_F);
    if (key_state_f_changed != key_state_f)
    {
        key_state_f = key_state_f_changed;
        if (key_state_f == GLFW_PRESS && key_state_left_control == GLFW_PRESS)
        {
            wireframe = !wireframe;
        }
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void processInput(GLFWwindow *window);
static void checkShaderProgramLinkStatus(unsigned int programId);
static void checkShaderCompileStatus(unsigned int shaderId);

void exercise2_setup(GLFWwindow * window)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* Compile Vertex Shader */
    unsigned int vertexShader;
    vertexShader =glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader);
    

    /* Compile Fragment Shader */
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader);

    /* Create Shader Program */
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkShaderProgramLinkStatus(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*
    The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms:

        GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
        GL_STATIC_DRAW: the data is set only once and used many times.
        GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    /*
    The function glVertexAttribPointer has quite a few parameters so let's carefully walk through them:

        . The first parameter specifies which vertex attribute we want to configure. Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0). This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
        . The next argument specifies the size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        . The third argument specifies the type of the data which is GL_FLOAT (a vec* in GLSL consists of floating point values).
        . The next argument specifies if we want the data to be normalized. If we're inputting integer data types (int, byte) and we've set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float. This is not relevant for us so we'll leave this at GL_FALSE.
        . The fifth argument is known as the stride and tells us the space between consecutive vertex attributes. Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride. Note that since we know that the array is tightly packed (there is no space between the next vertex attribute value) we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we'll get to see more examples of that later on.
        . The last parameter is of type void* and thus requires that weird cast. This is the offset of where the position data begins in the buffer. Since the position data is at the start of the data array this value is just 0. We will explore this parameter in more detail later on
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void exercise2_imgui(GLFWwindow * window)
{

}

int exercise2(GLFWwindow * window)
{
    processInput(window);

    if(wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    // ..:: Drawing code (in render loop) :: ..
    // 4. draw the object
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    return 0;

}


static void checkShaderProgramLinkStatus(unsigned int programId)
{
    int success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout<< "ERROR::SHADER::LINK_FAILED\n" << infoLog<<std::endl;
    }
}

static void checkShaderCompileStatus(unsigned int shaderId)
{
    int  success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout<< "ERROR::SHADER::COMPILATION_FAILED\nSHADER ID:"<<shaderId <<"\nINFO LOG:" << infoLog<<std::endl;
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
        glViewport(0, 0, width, height);
    }
}

static void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    processInputWireframe(window);
}