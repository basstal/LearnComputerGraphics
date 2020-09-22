#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include <shader.h>
#include <camera.h>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

float vertices[] = {
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

glm::vec3 cubePositions[] = {
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

glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  2.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

glm::vec3 pointLightRepresentColor[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.2f, 1.0f),
    glm::vec3(0.5f, 0.2f, 1.0f),
};

const int WIDTH = 800;
const int HEIGHT = 600;

Camera camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = (float)glfwGetTime();
bool moveMouse = true;

void frame_buffer_callback(GLFWwindow * window, int , int );
void scroll_callback(GLFWwindow *, double , double);
void mouse_callback(GLFWwindow * window, double xPos, double yPos);
void processInput(GLFWwindow *);
// unsigned int loadImage(const char * fileName, GLint format, bool, GLint);


int main()
{
    // init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Chapter2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR::CREATWINDOW::FAILED!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // init glad
    if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
    {
        std::cout << "ERROR::LOAD GL LOADER::FAILED!" << std::endl;
        return -1;
    }

    // set frame buffer size callback
    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    unsigned int diffuseTexture, specularTexture, colorSpecularTex, emissionTexture;
    // diffuseTexture = loadImage("container2.png", GL_RGBA, GL_FALSE, GL_REPEAT);
    // specularTexture = loadImage("container2_specular.png", GL_RGBA, GL_FALSE, GL_REPEAT);
    // colorSpecularTex = loadImage("lighting_maps_specular_color.png", GL_RGBA, GL_FALSE, GL_REPEAT);
    // emissionTexture = loadImage("matrix.jpg", GL_RGB, GL_FALSE, GL_REPEAT);

    unsigned int VAO, VBO;
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
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader shader = Shader("Shaders/2_1/ColorsVertexShader.vs", "Shaders/2_1/ColorsFragmentShader.fs", NULL);
    // Shader lampShader = Shader("VertexShader2.glsl", "LightFragmentShader.glsl");

    // glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

    // glEnable(GL_DEPTH_TEST);

    // while(!glfwWindowShouldClose(window))
    // {
    //     processInput(window);
        
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     glm::mat4 view = camera.GetViewMatrix();
    //     glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH/HEIGHT, 0.01f, 100.0f);
    
    //     lampShader.use();
    //     lampShader.setMat4("view", view);
    //     lampShader.setMat4("projection", projection);

        
    //     shader.use();

    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    //     glActiveTexture(GL_TEXTURE1);
    //     glBindTexture(GL_TEXTURE_2D, specularTexture);
    //     glActiveTexture(GL_TEXTURE2);
    //     glBindTexture(GL_TEXTURE_2D, colorSpecularTex);
    //     glActiveTexture(GL_TEXTURE3);
    //     glBindTexture(GL_TEXTURE_2D, emissionTexture);

    //     shader.setInt("material.diffuse", 0);
    //     shader.setInt("material.specular", 1);
    //     shader.setInt("material.colorSpecular", 2);
    //     shader.setInt("material.emission", 3);
    //     shader.setFloat("material.shininess", 32.0f);

    //     shader.setVec3("viewPos", camera.Position);

    //     shader.setVec3("dirLight.direction", glm::vec3(0.0f, 1.0f, 0.2f));
    //     shader.setVec3("dirLight.ambient", glm::vec3(0.00f));
    //     shader.setVec3("dirLight.diffuse", glm::vec3(0.2f));
    //     shader.setVec3("dirLight.specular", glm::vec3(1.0f));

    //     for ( int i = 0; i < 4; ++ i)
    //     {
    //         std::string prefix = "pointLights[" + std::to_string(i) + "]";
    //         shader.setVec3(prefix + ".position", pointLightPositions[i]);
    //         shader.setFloat(prefix + ".constant", 1.0f);
    //         shader.setFloat(prefix + ".linear", 0.07f);
    //         shader.setFloat(prefix + ".quadratic", 0.017f);

    //         shader.setVec3(prefix + ".ambient", glm::vec3(0.00f));
    //         shader.setVec3(prefix + ".diffuse", glm::vec3(0.2f));
    //         shader.setVec3(prefix + ".specular", glm::vec3(0.2f));
    //     }
    //     shader.setVec3("spotLight.direction", camera.Front);
    //     shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    //     shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    //     shader.setVec3("spotLight.ambient", glm::vec3(0.0f));
    //     shader.setVec3("spotLight.diffuse", glm::vec3(0.5f));
    //     shader.setVec3("spotLight.specular", glm::vec3(0.5f));


    //     shader.setMat4("view", view);
    //     shader.setMat4("projection", projection);

    //     shader.use();

    //     glBindVertexArray(VAO);

    //     for ( int i = 0; i < 10 ; ++ i)
    //     {

    //         glm::mat4 model;
    //         model = glm::rotate(model, glm::radians(30.0f) * i, glm::vec3(0.3f, 0.7f, 0.0f));
    //         model = glm::translate(model, cubePositions[i]);

    //         shader.setMat4("model", model);
    //         glDrawArrays(GL_TRIANGLES, 0, 36);
            
    //     }

    //     lampShader.use();
    //     for (int i = 0; i < 4; ++ i)
    //     {
    //         glm::mat4 model;
    //         model = glm::translate(model, pointLightPositions[i]);
    //         model = glm::scale(model, glm::vec3(0.2f));
            

    //         lampShader.setVec3("color", pointLightRepresentColor[i]);
    //         lampShader.setMat4("model", model);
    //         glDrawArrays(GL_TRIANGLES, 0, 36);
    //     }

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }
    glfwTerminate();
    return 0;
}

void frame_buffer_callback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window)
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
    
}

void scroll_callback(GLFWwindow *window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

void mouse_callback(GLFWwindow * window, double xPos, double yPos)
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

// unsigned int loadImage(const char * fileName, GLint format, bool verticalFlip, GLint wrapMode)
// {
//     unsigned int texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);
    
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     stbi_set_flip_vertically_on_load(verticalFlip);
//     int width, height, nrchannel;
//     unsigned char * data = stbi_load(fileName, &width, &height, &nrchannel, 0);
//     if ( data != NULL )
//     {
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "ERROR::LOAD IMAGE::FAILED!" << std::endl;
//     }
    
//     stbi_image_free(data);
//     return texture;
// }