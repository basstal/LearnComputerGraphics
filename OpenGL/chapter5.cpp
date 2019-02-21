#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <iostream>

#include <camera.h>
#include <shader.h>

const int HEIGHT = 780;
const int WIDTH = 1280;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Camera camera = Camera();


// 1-blinn phong
bool openBlinn = false;

float blinnPhong[] = {
    // positions            // normals         // texcoords
    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
    10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

using namespace std;

void frame_buffer_callback(GLFWwindow *, int , int);
void cursor_pos_callback(GLFWwindow *, double, double);
void mouse_scroll_callback(GLFWwindow *, double, double);
void processInput(GLFWwindow * window);
unsigned int loadImage(const char * path);


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "CHAPTER5", NULL, NULL);
    if (window == NULL)
    {
        cout << "ERROR::CREATE WINDOW:: FAILED!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if( ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
    {
        cout << "ERROR::GLAD LOADER INIT FAILED!" <<endl;
        glfwTerminate();
        return -1;
    }

    
    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    // 1-blinn phong
    Shader shaderPlane = Shader("VertexShaderPlane.glsl", "FragmentShaderPlane.glsl", "");
    unsigned int texture = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/wood.png");

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blinnPhong), blinnPhong, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)( 3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)( 6 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        // render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // 1 - blinn phong
        shaderPlane.use();
        shaderPlane.setMat4("model", glm::mat4(1.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

        shaderPlane.setMat4("view", view);
        shaderPlane.setMat4("projection", projection);

        shaderPlane.setFloat("shininess", 16.0f);
        shaderPlane.setBool("openBlinn", openBlinn);
        shaderPlane.setVec3("viewPos", camera.Position);
        shaderPlane.setInt("texture0", 0);

        shaderPlane.setVec3("dotLight.position", glm::vec3(0.0f));
        shaderPlane.setVec3("dotLight.ambient", glm::vec3(0.2f));
        shaderPlane.setVec3("dotLight.diffuse", glm::vec3(0.4f));
        shaderPlane.setVec3("dotLight.specular", glm::vec3(0.6f));

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void processInput(GLFWwindow * window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    // 1 -blinn phong
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        openBlinn = !openBlinn;
    }
}

void frame_buffer_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void cursor_pos_callback(GLFWwindow * window, double xPos, double yPos)
{
    if (firstMove)
    {
        lastX = (float)xPos;
        lastY = (float)yPos;
        firstMove = false;
    }

    float offsetX = (float)xPos - lastX;
    float offsetY = lastY - (float)yPos;
    lastX = (float)xPos;
    lastY = (float)yPos;

    camera.ProcessMouseMovement(offsetX, offsetY);
}

void mouse_scroll_callback(GLFWwindow * window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

unsigned int loadImage(const char * path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannel;
    unsigned char * data = stbi_load(path, &width, &height, &nrChannel, 0);
    if (data != NULL)
    {
        GLint format;
        if (nrChannel == 1)
            format = GL_RED;
        else if (nrChannel == 3)
            format = GL_RGB;
        else if (nrChannel == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        cout << "ERROR::LOAD IMAGE FAILED!" << endl;
    }
    stbi_image_free(data);


    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}