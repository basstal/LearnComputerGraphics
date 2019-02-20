#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>
#include <vector>
#include <map>

bool wireframe = false;

float skyboxVertices[] = {
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

float housePoints[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
    0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};  

float instancedQuadVertices[] = {
    // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
     0.05f,  0.05f,  0.0f, 1.0f, 1.0f		    		
}; 

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

float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
};

float grassVertices[] = {
    -0.5f,  -0.5f,  0.0f,   1.0f,   1.0f,
    -0.5f,  0.5f,   0.0f,   1.0f,   0.0f,
    0.5f,   -0.5f,  0.0f,   0.0f,   1.0f,
    0.5f,   0.5f,   0.0f,   0.0f,   0.0f,
};

float quadVertices[] = {  
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};	

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path, GLint);
void DrawScene(Shader);
void DrawModel(Shader, Model);
unsigned int LoadSkyboxTex(std::vector<std::string> skyboxTexs);
void DrawSkybox(Shader);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int cubeVAO, cubeVBO;
unsigned int grassVAO, grassVBO;
unsigned int planeVAO, planeVBO;
unsigned int quadVAO, quadVBO;
unsigned int frameBuffer;
unsigned int cubeTexture, floorTexture, grassTexture, windowTexture;
unsigned int skyboxVAO, skyboxVBO;
unsigned int skyboxTextures;
unsigned int houseVAO, houseVBO;
unsigned int instanceVAO, instanceVBO, instanceVBO1, instanceRockVBO;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glEnable(GL_STENCIL_TEST);
    // glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // build and compile shaders
    // -------------------------
    Shader shader("VertexShader4.glsl", "FragmentShader4.glsl", "");
    Shader simpleColorShader("VertexShader4.glsl", "SimpleColorFragmentShader.glsl", "");
    Shader quadShader("QuadVertexShader.glsl", "QuadFragmentShader.glsl", "");
    Shader skyboxShader("VertexShaderSkybox.glsl", "FragmentShaderSkybox.glsl", "");
    Shader instanceModelShader("VertexShaderInstanceModel.glsl", "FragmentShaderModel.glsl", "");
    Shader modelShader("VertexShaderModel.glsl", "FragmentShaderModel.glsl", "");
    Shader ubo1Shader("VertexShaderUBO.glsl", "FragmentShaderUBO1.glsl", "");
    Shader ubo2Shader("VertexShaderUBO.glsl", "FragmentShaderUBO2.glsl", "");
    Shader ubo3Shader("VertexShaderUBO.glsl", "FragmentShaderUBO3.glsl", "");
    Shader normalModelShader("VertexShaderModel.glsl", "FragmentShaderModelNormal.glsl", "GeometryShader.glsl");
    Shader instanceShader("VertexShaderInstance.glsl", "FragmentShaderInstance.glsl", "");
    // Shader ubo4Shader("VertexShaderUBO.glsl", "FragmentShaderUBO4.glsl", "GeometryShader.glsl");

    /*
        Remember: to specify vertices in a counter-clockwise winding order you need to visualize the triangle
        as if you're in front of the triangle and from that point of view, is where you set their order.
        
        To define the order of a triangle on the right side of the cube for example, you'd imagine yourself looking
        straight at the right side of the cube, and then visualize the triangle and make sure their order is specified
        in a counter-clockwise order. This takes some practice, but try visualizing this yourself and see that this
        is correct.
    */
    std::vector<glm::vec3> vegetation;
    vegetation.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    vegetation.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    vegetation.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    vegetation.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));

    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < vegetation.size(); ++i)
    {
        float distance = glm::length(camera.Position - vegetation[i]);
        sorted[distance] = vegetation[i];
    }

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // grass VAO
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);

    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), &grassVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glBindVertexArray(0);

    //quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
    glBindVertexArray(0);

    // gen frameBuffer
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // gen texture attachment for color buffer
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // gen renderBuffer attachment for depth and stencil buffer
    unsigned int renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAME BUFFER:: Framebuffer is not complete! " << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Skybox vao
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);

    // uniform buffer object
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    // 这里得到的index 是glUniformBlockBinding的第二个参数，第三个参数决定binding的具体下标值，通过这个方法同样下标值的binding可以通用一份数据
    unsigned int ubi1 = glGetUniformBlockIndex(ubo1Shader.ID, "Matrices");
    unsigned int ubi2 = glGetUniformBlockIndex(ubo2Shader.ID, "Matrices");
    unsigned int ubi3 = glGetUniformBlockIndex(ubo3Shader.ID, "Matrices");
    // unsigned int ubi4 = glGetUniformBlockIndex(ubo4Shader.ID, "Matrices");
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubi1, 0);
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubi2, 0);
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubi3, 0);
    // glUniformBlockBinding(GL_UNIFORM_BUFFER, ubi4, 0);

    unsigned int ubiModel = glGetUniformBlockIndex(modelShader.ID, "Matrices");
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubiModel, 0);

    unsigned int ubiInstanceModel = glGetUniformBlockIndex(instanceModelShader.ID, "Matrices");
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubiInstanceModel, 0);

    unsigned int ubiInstance = glGetUniformBlockIndex(instanceShader.ID, "Matrices");
    glUniformBlockBinding(GL_UNIFORM_BUFFER, ubiInstance, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 记得要glBindBufferRange或者glBindBufferBase，将某一个下标值和具体的buffer绑定
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    // house use Geometry Shader
    glGenVertexArrays(1, &houseVAO);
    glGenBuffers(1, &houseVBO);

    glBindVertexArray(houseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(housePoints), housePoints, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);

    // instance vertices
    glGenVertexArrays(1, &instanceVAO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(instanceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instancedQuadVertices), instancedQuadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glm::vec2 offsets[100];
    float offsetFix = 0.4f;
    int index = 0;
    for (int col = -10; col < 10; col += 2)
    {
        for (int row = -10 ; row < 10; row += 2)
        {
            float x = (row / 10.0f) + offsetFix;
            float y = (col / 10.0f) + offsetFix;
            offsets[index++] = glm::vec2(x, y);
        }
    }
    glGenBuffers(1, &instanceVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &offsets[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);

    // load textures
    // -------------
    cubeTexture  = loadTexture("resources/container.jpg", GL_REPEAT);
    floorTexture = loadTexture("resources/metal.png", GL_REPEAT);
    grassTexture = loadTexture("resources/grass.png", GL_CLAMP_TO_EDGE);
    windowTexture = loadTexture("resources/window.png", GL_REPEAT);
    // Model nanosuit(std::string("F:/Documents/OpenGL/Models/nanosuit_reflection/nanosuit.obj").c_str());
    // Model nanosuit(std::string("/Users/wangjunke/Documents/OpenGL/OpenGLResource/nanosuit/nanosuit.obj").c_str());
    Model planet(std::string("F:/Documents/OpenGL/Models/planet/planet.obj").c_str());
    Model rock(std::string("F:/Documents/OpenGL/Models/rock/rock.obj").c_str());


    // create 1000 amounts of rock transform matrices
    int amount = 100000;
    srand(glfwGetTime());
    glm::mat4 *modelMatrices = new glm::mat4[amount];
    float radius = 100.0f;
    float offset = 25.0f;

    for (int i = 0; i < amount ; ++i)
    {
        glm::mat4 trans;

        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100.0f)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100.0f)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100.0f)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        trans = glm::translate(trans, glm::vec3(x, y, z));

        float scale = (rand() % 20) / 100.0f + 0.05;
        trans = glm::scale(trans, glm::vec3(scale));

        float rotAngle = rand() % 360;
        trans = glm::rotate(trans, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = trans;
    }

    glGenBuffers(1, &instanceRockVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceRockVBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    for (int meshIdx = 0; meshIdx < rock.meshes.size(); ++meshIdx)
    {
        unsigned int VAO = rock.meshes[meshIdx].VAO;
        GLsizei vec4Size = sizeof(glm::vec4);
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));

        // 记得要切换instancing 的 location attribDivisor
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    std::vector<std::string> skyboxTexs = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg",
    };
    skyboxTextures = LoadSkyboxTex(skyboxTexs);

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    quadShader.use();
    quadShader.setInt("texture0", 0);
    
    if (!wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // glEnable(GL_PROGRAM_POINT_SIZE);
    // render loop
    // -----------
    while(!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

        // modelShader.use();
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(4.0f));
        // modelShader.setMat4("model", model);
        // planet.Draw(modelShader);

        // modelShader.use();
        // for (int i = 0; i < amount; ++i)
        // {
        //     modelShader.setMat4("model", modelMatrices[i]);
        //     rock.Draw(modelShader);
        // }

        instanceModelShader.use();
        for (int i = 0; i < rock.meshes.size(); ++i)
        {
            GLsizei count = rock.meshes[i].indices.size();
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0, amount);
        }

        // instanceShader.use();
        // glBindVertexArray(instanceVAO);
        
        // for (unsigned int index = 0; index < 100; ++ index)
        // {
        //     std::stringstream ss;
        //     ss << index;
        //     instanceShader.setVec2(("offset[" + ss.str() + "]"), offsets[index]);
        // }

        // glm::mat4 model = glm::mat4(1.0f);
        // instanceShader.setMat4("model", model);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);


        // // glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // // glm::mat4 view = camera.GetViewMatrix();

        // modelShader.use();

        // modelShader.setFloat("material.shininess", 0.5f);
    
        // // modelShader.setMat4("view", view);
        // // modelShader.setMat4("projection", projection);
        // // modelShader.setVec3("cameraPos", camera.Position);
        // // modelShader.setFloat("time", currentFrame);
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -1.75f, -10.0f));
        // // model = glm::scale(model, glm::vec3(0.5f));
        // modelShader.setMat4("model", model);

        // nanosuit.Draw(modelShader);

        // normalModelShader.use();
        // normalModelShader.setMat4("model", model);
        // nanosuit.Draw(normalModelShader);



        // glBindVertexArray(houseVAO);
        // ubo1Shader.use();
        // glm::mat4 model = glm::mat4(1.0f);
        // ubo1Shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // ubo2Shader.use();
        // model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
        // ubo2Shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // ubo3Shader.use();
        // model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        // ubo3Shader.setMat4("model", model);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        
        
        // glDrawArrays(GL_POINTS, 0, 4);

        // glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(GL_LEQUAL);
        // DrawScene(shader);
        // DrawModel(modelShader, nanosuit);

        // DrawSkybox(skyboxShader);

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glDisable(GL_DEPTH_TEST);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        
        // // use frameBuffer as texture for quad
        // quadShader.use();
        // glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        // glBindVertexArray(quadVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindVertexArray(0);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path, GLint wrapMode )
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void DrawScene(Shader shader)
{
    
    shader.use();

    
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);

    glDisable(GL_CULL_FACE);
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glBindVertexArray(cubeVAO);
    // glBindTexture(GL_TEXTURE_2D, cubeTexture); 	
    // model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    // shader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    // shader.setMat4("model", model);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // glBindVertexArray(0);
    // glDisable(GL_CULL_FACE);
    
    float scale = 1.1f;
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture); 	
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    shader.setMat4("model", model);

    glDrawArrays(GL_POINTS, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    shader.setMat4("model", model);

    glDrawArrays(GL_POINTS, 0, 36);
    glBindVertexArray(0);
    // glStencilMask(0xff);
    // glEnable(GL_DEPTH_TEST);
}


void DrawModel(Shader shader, Model nanosuit)
{
    
    shader.use();

    shader.setFloat("material.shininess", 0.5f);
    
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("cameraPos", camera.Position);
    shader.setInt("material.texture1", 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextures);


    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.2f));
    shader.setMat4("model", model);

    nanosuit.Draw(shader);
}


unsigned int LoadSkyboxTex(std::vector<std::string> skyboxTexs)
{
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    int width, height, nrChannel;
    for (unsigned int i = 0 ; i < skyboxTexs.size(); ++i)
    {
        unsigned char * data = stbi_load(skyboxTexs[i].c_str(), &width, &height, &nrChannel, 0);
        if ( data )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "ERROR::LOAD SKYBOX TEXTURE FALIED" << std::endl;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return skyboxTex;
}


void DrawSkybox(Shader shader)
{
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);
    shader.use();

    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.01f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setInt("cubeMap", 0);

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextures);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // glDepthMask(GL_TRUE);
    // glEnable(GL_DEPTH_TEST);
}