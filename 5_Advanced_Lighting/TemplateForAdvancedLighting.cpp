#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <iostream>
#include <vector>
#include <random>

#include <camera.h>
#include <Shader.h>
#include <model.h>

bool wireframe = false;


const int HEIGHT = 780;
const int WIDTH = 1280;

const int SHADOW_WIDTH = 1024;
const int SHADOW_HEIGHT = 1024;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;

float lastFrame = 0.0f;
float deltaTime = 0.0f;

Camera camera = Camera();


// 1-blinn phong
bool openBlinn = false;
bool glBlinnPressed = false;

// 2-gamma correction
bool openGammaCorrection = false;
bool glGammaCorrection = false;

// 4 parallax mapping
bool enableNormalMap = true;
bool normalMappingSwitch = false;

// 8 SSAO
bool enableSSAO = false;
bool SSAOSwitch = false;

float heightScale = 0.1f;
float exposure = 1.0f;

float blinnPhong[] = {
    // positions            // normals         // texcoords
    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

    10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
    10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};


float planeVertices[] = {
    // positions            // normals         // texcoords
    25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

    25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
    25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 10.0f
};

using namespace std;

void frame_buffer_callback(GLFWwindow *, int , int);
void cursor_pos_callback(GLFWwindow *, double, double);
void mouse_scroll_callback(GLFWwindow *, double, double);
void processInput(GLFWwindow * window);
unsigned int loadImage(const char * path, bool openGammaCorrection);
void renderCube();
void renderQuad();
void renderScene(const Shader &shader, unsigned int planeVAO);
void renderScene3D(const Shader &shader);
void renderQuadSimple();
void renderCubeSimple();
float lerp(float, float, float);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

    if (!wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LEQUAL);
    
//     glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
//     glfwSetCursorPosCallback(window, cursor_pos_callback);
//     glfwSetScrollCallback(window, mouse_scroll_callback);

//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//     // unsigned int textureGammaCorrection = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/wood.png", true);
//     // unsigned int textureGammaIncorrect = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/wood.png", false);
//     // // 3 normal mapping
//     // unsigned int wall = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/bricks2.jpg", false);
//     // unsigned int wallNormal = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/bricks2_normal.jpg", false);
//     // unsigned int wallParallaxMapping = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/bricks2_disp.jpg", false);

//     // unsigned int wall = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/brickwall.jpg", false);
//     // unsigned int wallNormal = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/brickwall_normal.jpg", false);

//     // unsigned int toy = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/toy_box_diffuse.png", false);
//     // unsigned int toy = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/wood.png", false);
//     // unsigned int toy = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/window.png", false);
    
//     // unsigned int toyNormal = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/toy_box_normal.png", false);
//     // unsigned int toyHeight = loadImage("/Users/wangjunke/Documents/OpenGL/OpenGL/resources/toy_box_disp.png", false);
    
//     // unsigned int toy = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/toy_box_diffuse.png", false);
//     // unsigned int toyNormal = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/toy_box_normal.png", false);
//     // unsigned int toyHeight = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/toy_box_disp.png", false);

//     // unsigned int woodTexture = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/wood.png", true);
//     // unsigned int container = loadImage("F:/Documents/OpenGL/OpenGL/OpenGL/resources/container2.png", true);

    // 7 Deferred Shading
    // Model nanosuit = Model("/Users/wangjunke/Documents/OpenGL/OpenGLResource/nanosuit/nanosuit.obj");
    Model nanosuit("F:/Documents/OpenGL/Models/nanosuit/nanosuit.obj", false);

    
    // // 1-blinn phong
    // Shader shaderPlane = Shader("VertexShaderPlane.glsl", "FragmentShaderPlane.glsl", "");
    // // 2 shadow mapping
    // Shader shaderSimpleShadow = Shader("VertexShaderShadow1.glsl", "FragmentShaderShadow1.glsl", "");
    // Shader mappingShadowShader = Shader("VertexShaderShadow2.glsl", "FragmentShaderShadow2.glsl", "");

    // // 2-2 point shadow
    // Shader pointShadowShader = Shader("VertexShaderShadow3.glsl", "FragmentShaderShadow3.glsl", "GeometryShaderShadow3.glsl");
    // Shader pointShadowDrawShader = Shader("VertexShaderShadow4.glsl", "FragmentShaderShadow4.glsl", "");

    // // 3 normal mapping
    // Shader normalMappingShader = Shader("VertexShaderNormalMapping.glsl", "FragmentShaderNormalMapping.glsl", "");
    // // 4 parallax mapping
    // Shader parallaxMappingShader = Shader("VertexShaderParallaxMapping.glsl", "FragmentShaderParallaxMapping.glsl", "");

    // // 5 HDR
    // Shader hdrObjShader = Shader("VertexShaderHDROBJ.glsl", "FragmentShaderHDROBJ.glsl", "");
    // Shader hdrQuadShader = Shader("VertexShaderHDR.glsl", "FragmentShaderHDR.glsl", "");

    // // 6 Bloom
    // Shader bloomShader = Shader("VertexShaderBloom.glsl", "FragmentShaderBloom.glsl", "");
    // Shader gaussianBlurShader = Shader("VertexShaderGaussianBlur.glsl", "FragmentShaderGaussianBlur.glsl", "");
    // Shader bloomQuadShader = Shader("VertexShaderBloomQuad.glsl", "FragmentShaderBloomQuad.glsl", "");
    // Shader shaderLight = Shader("VertexShaderLight1.glsl", "FragmentShaderLight1.glsl", "");

    // // 7 Deferred Shading
    // Shader deferredShader = Shader("VertexShaderDeferred.glsl", "FragmentShaderDeferred.glsl", "");
    // Shader quadShader = Shader("VertexShaderQuad1.glsl", "FragmentShaderQuad1.glsl", "");
    // Shader shaderLight2 = Shader("VertexShaderLight2.glsl", "FragmentShaderLight2.glsl", "");


    // 8 SSAO
    Shader shaderGeometryPass = Shader("VertexShaderDraw.glsl", "FragmentShaderDraw.glsl", "");
    Shader ssaoShader = Shader("VertexShaderSSAO.glsl", "FragmentShaderSSAO.glsl", "");
    Shader ssaoBlurShader = Shader("VertexShaderSSAO.glsl", "FragmentShaderSSAOBlur.glsl", "");
    Shader ssaoQuadShader = Shader("VertexShaderQuad1.glsl", "FragmentShaderQuad2.glsl", "");


    // unsigned int planeVAO, planeVBO;
    // glGenVertexArrays(1, &planeVAO);
    // glGenBuffers(1, &planeVBO);

    // glBindVertexArray(planeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)( 3 * sizeof(float)));
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)( 6 * sizeof(float)));
    
//     // glBindBuffer(GL_ARRAY_BUFFER, 0);
//     // glBindVertexArray(0);
    
//     // // 2 shadow mapping
//     // unsigned int shadowFBO;
//     // glGenFramebuffers(1, &shadowFBO);

//     // unsigned int depthTexture;
//     // glGenTextures(1, &depthTexture);
//     // glBindTexture(GL_TEXTURE_2D, depthTexture);
//     // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//     // float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
//     // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     // glBindTexture(GL_TEXTURE_2D, 0);

//     // glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//     // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
//     // glDrawBuffer(GL_NONE);
//     // glReadBuffer(GL_NONE);
//     // if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//     //     cout << "ERROR:: FRAME BUFFER INIT FAILED! " << endl;
//     // glBindFramebuffer(GL_FRAMEBUFFER, 0);

//     // // 2-2 point shadow
//     // unsigned int shadowPointFBO;
//     // glGenFramebuffers(1, &shadowPointFBO);
//     // glBindFramebuffer(GL_FRAMEBUFFER, shadowPointFBO);

//     // unsigned int cubeMap;
//     // glGenTextures(1, &cubeMap);
//     // glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

//     // for (unsigned int i = 0; i < 6 ; ++i)
//     // {
//     //     glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     // }

//     // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//     // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//     // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap, 0);
//     // glDrawBuffer(GL_NONE);
//     // glReadBuffer(GL_NONE);
    

    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     cout << "ERROR:: FRAME BUFFER ATTACHMENT FAILED!"<< endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // unsigned int HDRFBO;
    // glGenFramebuffers(1, &HDRFBO);

    // unsigned int HDRTexture, depthTexture;
    // glGenTextures(1, &HDRTexture);
    // glBindTexture(GL_TEXTURE_2D, HDRTexture);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // unsigned int rboDepth;
    // glGenRenderbuffers(1, &rboDepth);
    // glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);

    // glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRTexture, 0);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     cout <<"ERROR:: FRAME BUFFER INIT FAILED!" << endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // // 6 Bloom
    // unsigned int bloomFBO;
    // unsigned int colorBuffers[2];
    // glGenFramebuffers(1, &bloomFBO);
    // glGenTextures(2, colorBuffers);

    // glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
    // for (unsigned int i = 0 ; i < 2; ++i)
    // {
    //     glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    // }

    // unsigned int renderBuffer;
    // glGenRenderbuffers(1, &renderBuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
    // glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    // glDrawBuffers(2, attachments);

    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     cout << "ERROR::FRAME BUFFER INIT FAILED!" << endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unsigned int pingpongFBO[2];
    // glGenFramebuffers(2, pingpongFBO);
    // unsigned int pingpongBuffer[2];
    // glGenTextures(2, pingpongBuffer);
    // for (unsigned int i = 0; i < 2; ++i)
    // {
    //     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    //     glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
    //     glBindTexture(GL_TEXTURE_2D, 0);
    //     if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //         cout << "ERROR::FRAME BUFFER INIT FAILED!" << endl;
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }

    // // 7 Deferred shading
    // unsigned int cacheDepthFBO;
    // glGenFramebuffers(1, &cacheDepthFBO);
    // glBindFramebuffer(GL_FRAMEBUFFER, cacheDepthFBO);
    // unsigned int depthTex;
    // glGenTextures(1, &depthTex);
    // glBindTexture(GL_TEXTURE_2D, depthTex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // // glDrawBuffer(GL_NONE);
    // // glReadBuffer(GL_NONE);
    // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     cout << "ERROR::FRAME BUFFER INIT FAILED! CACHE DEPTH FBO" <<endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // unsigned int gBufferFBO;
    // glGenFramebuffers(1, &gBufferFBO);
    // glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

    // unsigned int textures[3];
    // glGenTextures(3, textures);
    // for (unsigned int i = 0; i < 3 ; ++i)
    // {
    //     glBindTexture(GL_TEXTURE_2D, textures[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, (i > 1 ? GL_RGBA: GL_RGB16F), WIDTH, HEIGHT, 0, i> 1? GL_RGBA : GL_RGB, i> 1? GL_UNSIGNED_BYTE : GL_FLOAT, NULL);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    // }

    // // unsigned int gPosition, gNormal, gAlbedoSpec;
    // // // position color buffer
    // // glGenTextures(1, &gPosition);
    // // glBindTexture(GL_TEXTURE_2D, gPosition);
    // // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // // // normal color buffer
    // // glGenTextures(1, &gNormal);
    // // glBindTexture(GL_TEXTURE_2D, gNormal);
    // // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // // // color + specular color buffer
    // // glGenTextures(1, &gAlbedoSpec);
    // // glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    // // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
    // glDrawBuffers(3, attachments);

    // unsigned int depthRB;
    // glGenRenderbuffers(1, &depthRB);
    // glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

    // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //     cout << "ERROR::FRAME BUFFER INIT FAILED!" <<endl;
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // 8 SSAO
    unsigned int drawFBO;
    glGenFramebuffers(1, &drawFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, drawFBO);

    unsigned int gPosition, gNormal, gAlbedoSpec;
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    unsigned int attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    unsigned int renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR:: DRAW FBO INIT FAILED!"<<endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uniform_real_distribution<float> randomFloat(0.0, 1.0f);
    default_random_engine generator;
    vector<glm::vec3> ssaoKernel;
    for (unsigned int i = 0; i < 64 ; ++i)
    {
        glm::vec3 sample(
            randomFloat(generator) * 2.0 - 1.0,
            randomFloat(generator) * 2.0 - 1.0,
            randomFloat(generator));
        sample = glm::normalize(sample);
        sample *= randomFloat(generator);
        float scale = (float) i / 64.0f;
        scale = lerp(0.1, 1.0, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0 ; i < 16; ++i)
    {
        glm::vec3 noise(
            randomFloat(generator) * 2.0 - 1.0,
            randomFloat(generator) * 2.0 - 1.0,
            0.0
        );
        ssaoNoise.push_back(noise);
    }

    unsigned int noiseTex;
    glGenTextures(1, &noiseTex);
    glBindTexture(GL_TEXTURE_2D, noiseTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned int ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR:: SSAO FBO INIT FAILED!" <<endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned int ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    unsigned int blurTex;
    glGenTextures(1, &blurTex);
    glBindTexture(GL_TEXTURE_2D, blurTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR:: SSAO BLUR FBO INIT FAILED!" <<endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // glm::vec3 lightPos = glm::vec3(0.0f);
    // float near_plane = 1.0f, far_plane = 25.0f;
    // glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH/ (float)SHADOW_HEIGHT, near_plane, far_plane);
    // vector<glm::mat4> shadowTransforms;
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
    // shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

    // pointShadowShader.use();
    // for (unsigned int i = 0; i < 6; ++i)
    //     pointShadowShader.setMat4("cubeMapMatrices[" + to_string(i) + "]", shadowTransforms[i]);

    // pointShadowShader.setFloat("far_plane", far_plane);
    // pointShadowShader.setVec3("lightPos", lightPos);

    // pointShadowDrawShader.use();
    // pointShadowDrawShader.setVec3("lightPos", lightPos);
    // pointShadowDrawShader.setFloat("far_plane", far_plane);
    // pointShadowDrawShader.setInt("diffuseTexture", 1);
    // pointShadowDrawShader.setInt("shadowCubeMap", 0);

    // // 3 normal mapping
    // normalMappingShader.use();
    // normalMappingShader.setInt("diffuseTexture", 0);
    // normalMappingShader.setInt("normalMapping", 1);
    // lightPos = glm::vec3(0.5f, 1.0f, 0.3f);

    // parallaxMappingShader.use();
    // parallaxMappingShader.setInt("diffuseTexture", 0);
    // parallaxMappingShader.setInt("normalMapping", 1);
    // parallaxMappingShader.setInt("parallaxMapping", 2);
    // parallaxMappingShader.setVec3("lightPos", lightPos);

    // // 5 HDR
    // hdrObjShader.use();
    // hdrObjShader.setInt("texture0", 0);

    // hdrQuadShader.use();
    // hdrQuadShader.setInt("hdrTexture", 0);
    // // positions
    // std::vector<glm::vec3> lightPositions;
    // lightPositions.push_back(glm::vec3( 0.0f,  0.0f, 49.5f)); // back light
    // lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    // lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
    // lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));
    // // colors
    // std::vector<glm::vec3> lightColors;
    // lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    // lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    // lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    // lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));
    

    // // 6 bloom
    // bloomQuadShader.use();
    // bloomQuadShader.setInt("texture0", 0);
    // bloomQuadShader.setInt("brightness0", 1);
    // gaussianBlurShader.use();
    // gaussianBlurShader.setInt("image", 0);
    // bloomShader.use();
    // bloomShader.setInt("texture0", 0);

    // std::vector<glm::vec3> lightPositions;
    // lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    // lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    // lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    // lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
    // // colors
    // std::vector<glm::vec3> lightColors;
    // lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
    // lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
    // lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
    // lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));

    // 7 Deferred Shading
    // std::vector<glm::vec3> objectPositions;
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));

    // quadShader.use();
    // quadShader.setInt("gPosition", 0);
    // quadShader.setInt("gNormal", 1);
    // quadShader.setInt("gAlbedoSpec", 2);

    // // lighting info
    // // -------------
    // const unsigned int NR_LIGHTS = 32;
    // std::vector<glm::vec3> lightPositions;
    // std::vector<glm::vec3> lightColors;
    // srand(13);
    // for (unsigned int i = 0; i < NR_LIGHTS; i++)
    // {
    //     // calculate slightly random offsets
    //     float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
    //     float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
    //     float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
    //     lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
    //     // also calculate random color
    //     float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    //     float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    //     float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    //     lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    // }

    // 8 SSAO 
    ssaoShader.use();
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("noiseTex", 2);
    ssaoShader.setFloat("radians", 0.5);
    ssaoShader.setFloat("bias", 0.025);


    ssaoBlurShader.use();
    ssaoBlurShader.setInt("ssaoInput", 0);

    // lighting info
    // -------------
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

    ssaoQuadShader.use();
    ssaoQuadShader.setInt("gPosition", 0);
    ssaoQuadShader.setInt("gNormal", 1);
    ssaoQuadShader.setInt("gAlbedoSpec", 2);
    ssaoQuadShader.setInt("gAmbientOcclusion", 3);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
//         // render
//         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//         // 1 - blinn phong
//         // shaderPlane.use();
//         // shaderPlane.setMat4("model", glm::mat4(1.0f));
//         // glm::mat4 view = camera.GetViewMatrix();
//         // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

//         // shaderPlane.setMat4("view", view);
//         // shaderPlane.setMat4("projection", projection);

//         // shaderPlane.setFloat("shininess", 16.0f);
//         // shaderPlane.setBool("openBlinn", openBlinn);
//         // shaderPlane.setVec3("viewPos", camera.Position);
//         // shaderPlane.setInt("texture0", 0);

//         // shaderPlane.setVec3("dotLight.position", glm::vec3(0.0f));
//         // shaderPlane.setVec3("dotLight.ambient", glm::vec3(0.2f));
//         // shaderPlane.setVec3("dotLight.diffuse", glm::vec3(0.4f));
//         // shaderPlane.setVec3("dotLight.specular", glm::vec3(0.6f));

//         // glBindVertexArray(planeVAO);
//         // glBindTexture(GL_TEXTURE_2D, (openGammaCorrection? textureGammaCorrection : textureGammaIncorrect));
//         // glDrawArrays(GL_TRIANGLES, 0, 6);
//         // glBindVertexArray(0);


//         // cout << (openGammaCorrection ? "Gamma Correction\\" : "Gamma Incorrect\\") << (openBlinn ? "Blinn Phong" : "Phong") << endl;


//         // 2 shadow mapping

//         // glEnable(GL_CULL_FACE);
//         // glCullFace(GL_BACK);
//         // float near_plane = 1.0f, far_plane = 7.5f;
//         // glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//         // glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
//         //                 glm::vec3(0.f, 0.f, 0.f),
//         //                 glm::vec3(0.f, 1.f, 0.f));

//         // glm::mat4 lightSpaceMatrix = lightProjection * view;

//         // shaderSimpleShadow.use();
//         // shaderSimpleShadow.setMat4("lightSpaceMatrix", lightSpaceMatrix);

//         // glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//         // glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
//         // glClear(GL_DEPTH_BUFFER_BIT);
//         // renderScene(shaderSimpleShadow, planeVAO);
//         // glDisable(GL_CULL_FACE);
        

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glViewport(0, 0, WIDTH, HEIGHT);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // mappingShadowShader.use();
        // mappingShadowShader.setInt("shadowMap", 1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, depthTexture);

        // glm::mat4 porj = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/ (float)HEIGHT, 0.1f, 100.f);
        // glm::mat4 cameraView = camera.GetViewMatrix();
        // mappingShadowShader.setMat4("projection", porj);
        // mappingShadowShader.setMat4("view", cameraView);
        // mappingShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // mappingShadowShader.setInt("diffuseTexture", 0);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, textureGammaCorrection);

        // mappingShadowShader.setVec3("lightPos", glm::vec3(-2.0f, 4.0f, -1.0f));
        // mappingShadowShader.setVec3("viewPos", camera.Position);
        // renderScene(mappingShadowShader, planeVAO);


        // 2-2 point shadow
        // pointShadowShader.use();
        // glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        // glBindFramebuffer(GL_FRAMEBUFFER, shadowPointFBO);
        // glClear(GL_DEPTH_BUFFER_BIT);
        // renderScene3D(pointShadowShader);

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glViewport(0, 0, WIDTH, HEIGHT);
        // pointShadowDrawShader.use();
        // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // pointShadowDrawShader.setVec3("viewPos", camera.Position);

        // glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.01f, 100.0f);
        // pointShadowDrawShader.setMat4("projection", proj);
        // glm::mat4 view = camera.GetViewMatrix();
        // pointShadowDrawShader.setMat4("view", view);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, textureGammaCorrection);
        // renderScene3D(pointShadowDrawShader);


        // 3 normal mapping

        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

        // glm::mat4 view = camera.GetViewMatrix();

        // normalMappingShader.use();

        // normalMappingShader.setMat4("projection", projection);
        // normalMappingShader.setMat4("view", view);
        // normalMappingShader.setVec3("lightPos", lightPos);
        // normalMappingShader.setVec3("viewPos", camera.Position);
        // glm::mat4 model = glm::mat4(1.0f);
        // // model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1.0, 0.0, 1.0));
        // model = glm::rotate(model, (float)glfwGetTime() * -0.2f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        // normalMappingShader.setMat4("model", model);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, wall);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, wallNormal);

        // renderQuad();


        // 4 parallax mapping
        // parallaxMappingShader.use();
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        // glm::mat4 view = camera.GetViewMatrix();
        // parallaxMappingShader.setMat4("view", view);
        // parallaxMappingShader.setMat4("projection", projection);
        // parallaxMappingShader.setMat4("model", model);
        // parallaxMappingShader.setVec3("viewDir", camera.Position);
        // parallaxMappingShader.setBool("enableNormalMap", enableNormalMap);
        // parallaxMappingShader.setFloat("heightScale", heightScale);

        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, toy);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, toyNormal);
        // glActiveTexture(GL_TEXTURE2);
        // glBindTexture(GL_TEXTURE_2D, toyHeight);
        // renderQuad();

        // 5 HDR
        // hdrObjShader.use();
        // glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
        // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        // glm::mat4 view = camera.GetViewMatrix();
        // hdrObjShader.setMat4("projection", projection);
        // hdrObjShader.setMat4("view", view);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, woodTexture);
        // for( unsigned int i = 0; i < lightPositions.size(); ++i)
        // {
        //     hdrObjShader.setVec3("lights[" + to_string(i) + "].position", lightPositions[i]);
        //     hdrObjShader.setVec3("lights[" + to_string(i) + "].color", lightColors[i]);
        // }
        // hdrObjShader.setVec3("viewPos", camera.Position);
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
        // model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
        // hdrObjShader.setMat4("model", model);
        // hdrObjShader.setBool("inverseNormals", true);
        // renderCubeSimple();

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // hdrQuadShader.use();
        // hdrQuadShader.setFloat("exposure", exposure);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, HDRTexture);
        // renderQuadSimple();

        // // 6 Bloom
        // glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
        // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        // glm::mat4 view = camera.GetViewMatrix();
        // glm::mat4 model = glm::mat4(1.0f);
        // bloomShader.use();
        // bloomShader.setMat4("projection", projection);
        // bloomShader.setMat4("view", view);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, woodTexture);
        // // set lighting uniforms
        // for (unsigned int i = 0; i < lightPositions.size(); i++)
        // {
        //     bloomShader.setVec3("lights[" + to_string(i) + "].position", lightPositions[i]);
        //     bloomShader.setVec3("lights[" + to_string(i) + "].color", lightColors[i]);
        // }
        // bloomShader.setVec3("viewPos", camera.Position);
        // // create one large cube that acts as the floor
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
        // model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();
        // // then create multiple cubes as the scenery
        // glBindTexture(GL_TEXTURE_2D, container);
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        // model = glm::scale(model, glm::vec3(0.5f));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        // model = glm::scale(model, glm::vec3(0.5f));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
        // model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
        // model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        // model = glm::scale(model, glm::vec3(1.25));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
        // model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
        // model = glm::scale(model, glm::vec3(0.5f));
        // bloomShader.setMat4("model", model);
        // renderCubeSimple();

        // // finally show all the light sources as bright cubes
        // shaderLight.use();
        // shaderLight.setMat4("projection", projection);
        // shaderLight.setMat4("view", view);

        // for (unsigned int i = 0; i < lightPositions.size(); i++)
        // {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, glm::vec3(lightPositions[i]));
        //     model = glm::scale(model, glm::vec3(0.25f));
        //     shaderLight.setMat4("model", model);
        //     shaderLight.setVec3("lightColor", lightColors[i]);
        //     renderCubeSimple();
        // }
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // bool firstIteration = true, horizontal = true;
        // unsigned int amount = 10;
        // gaussianBlurShader.use();
        // for (unsigned int i = 0; i < amount; ++i)
        // {
        //     glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        //     gaussianBlurShader.setBool("horizontal", horizontal);
        //     glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongBuffer[!horizontal]);
        //     renderQuadSimple();
        //     horizontal = !horizontal;
        //     if( firstIteration )
        //         firstIteration = false;
        // }

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // bloomQuadShader.use();
        // bloomQuadShader.setFloat("exposure", exposure);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
        // renderQuadSimple();

        // // 7 Deferred Shading
        // glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // deferredShader.use();
        // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.f);
        // glm::mat4 view = camera.GetViewMatrix();
        // deferredShader.setMat4("projection", projection);
        // deferredShader.setMat4("view", view);
        // for (unsigned int i = 0 ; i < objectPositions.size(); ++i)
        // {

        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, objectPositions[i]);
        //     model = glm::scale(model, glm::vec3(0.25f));
        //     deferredShader.setMat4("model", model);
        //     nanosuit.Draw(deferredShader);
        // }
        

        

        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);
        // quadShader.use();
        // for (unsigned int i = 0; i < 3 ; ++ i)
        // {
        //     glActiveTexture(GL_TEXTURE0 + i);
        //     glBindTexture(GL_TEXTURE_2D, textures[i]);
        // }
        // // glActiveTexture(GL_TEXTURE0);
        // // glBindTexture(GL_TEXTURE_2D, gPosition);
        // // glActiveTexture(GL_TEXTURE1);
        // // glBindTexture(GL_TEXTURE_2D, gNormal);
        // // glActiveTexture(GL_TEXTURE2);
        // // glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        // quadShader.setVec3("viewPos", camera.Position);
        // for (unsigned int i = 0 ; i < lightPositions.size(); ++i)
        // {
        //     quadShader.setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
        //     quadShader.setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
        //     // update attenuation parameters and calculate radius
        //     const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
        //     const float linear = 0.7;
        //     const float quadratic = 1.8;
        //     quadShader.setFloat("lights[" + to_string(i) + "].Linear", linear);
        //     quadShader.setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);
        // }
        // renderQuadSimple();

        // glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO);
        // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // shaderLight2.use();
        // shaderLight2.setMat4("projection", projection);
        // shaderLight2.setMat4("view", view);
        // for (unsigned int i = 0; i < lightPositions.size(); i++)
        // {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, glm::vec3(lightPositions[i]));
        //     model = glm::scale(model, glm::vec3(0.25f));
        //     shaderLight2.setMat4("model", model);
        //     shaderLight2.setVec3("lightColor", lightColors[i]);
        //     renderCubeSimple();
        // }


        // 8 SSAO
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/ (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, drawFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        // room cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
        shaderGeometryPass.setMat4("model", model);
        shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
        renderCubeSimple();
        shaderGeometryPass.setInt("invertedNormals", 0); 
        // nanosuit model on the floor
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shaderGeometryPass.setMat4("model", model);
        nanosuit.Draw(shaderGeometryPass);

        // use G-buffer to render SSAO texture
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTex);
        // send kernel sample to shader
        ssaoShader.setMat4("projection", projection);
        for (unsigned int i = 0 ; i < ssaoKernel.size(); ++i)
        {
            ssaoShader.setVec3("samples[" + to_string(i) + "]", ssaoKernel[i]);
        }
        renderQuadSimple();

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoBlurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        renderQuadSimple();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoQuadShader.use();
        glm::vec3 lightPosView = glm::vec3(view * glm::vec4(lightPos, 1.0));
        ssaoQuadShader.setVec3("lightPos", lightPosView);
        ssaoQuadShader.setVec3("lightColor", lightColor);
        // Update attenuation parameters
        const float linear    = 0.09;
        const float quadratic = 0.032;
        ssaoQuadShader.setFloat("linear", linear);
        ssaoQuadShader.setFloat("quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, blurTex);
        ssaoQuadShader.setBool("enableSSAO", enableSSAO);
        renderQuadSimple();



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
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !glBlinnPressed)
    {
        openBlinn = !openBlinn;
        glBlinnPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        glBlinnPressed = false;
    }

    // 2-gamma correction
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !glGammaCorrection)
    {
        openGammaCorrection = !openGammaCorrection;
        glGammaCorrection = true;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        glGammaCorrection = false;
    }

    // 4 parallax mapping
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !SSAOSwitch)
    {
        enableSSAO = !enableSSAO;
        SSAOSwitch = true;
    }
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
    {
        SSAOSwitch = false;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        // if (heightScale > 0.0f)
        //     heightScale -= 0.0005f;
        // else
        //     heightScale = 0.0f;

        if(exposure > 0.0f)
            exposure -= 0.05f;
        else
            exposure = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        // if (heightScale < 1.0f)
        //     heightScale += 0.0005f;
        // else
        //     heightScale = 1.0f;
        if (exposure < 5.0f)
            exposure += 0.05f;
        else
            exposure = 5.0f;
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

unsigned int loadImage(const char * path, bool openGammaCorrection)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width, height, nrChannel;
    unsigned char * data = stbi_load(path, &width, &height, &nrChannel, 0);
    if (data != NULL)
    {
        GLint format;
        GLint internalFormat;

        if (nrChannel == 1)
            internalFormat = format = GL_RED;
        else if (nrChannel == 3)
        {
            format = GL_RGB;
            internalFormat = openGammaCorrection ? GL_SRGB : GL_RGB;
        }
        else if (nrChannel == 4)
        {
            internalFormat = openGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;   
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

void renderCubeSimple()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuadSimple()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void renderQuad()
{
    if (quadVAO == 0)
    {

         // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);  
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent1;
        tangent1.x = f * (edge1.x * deltaUV2.y - edge2.x * deltaUV1.y);
        tangent1.y = f * (edge1.y * deltaUV2.y - edge2.y * deltaUV1.y);
        tangent1.z = f * (edge1.z * deltaUV2.y - edge2.z * deltaUV1.y);
        tangent1 = glm::normalize(tangent1);

        glm::vec3 bitangent1;
        bitangent1.x = f * (edge2.x * deltaUV1.x - edge1.x * deltaUV2.x);
        bitangent1.y = f * (edge2.y * deltaUV1.x - edge1.y * deltaUV2.x);
        bitangent1.z = f * (edge2.z * deltaUV1.x - edge1.z * deltaUV2.x);
        bitangent1 = glm::normalize(bitangent1);

        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;

        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        glm::vec3 tangent2;
        tangent2.x = f * (edge1.x * deltaUV2.y - edge2.x * deltaUV1.y);
        tangent2.y = f * (edge1.y * deltaUV2.y - edge2.y * deltaUV1.y);
        tangent2.z = f * (edge1.z * deltaUV2.y - edge2.z * deltaUV1.y);
        tangent2 = glm::normalize(tangent2);

        glm::vec3 bitangent2;
        bitangent2.x = f * (edge2.x * deltaUV1.x - edge1.x * deltaUV2.x);
        bitangent2.y = f * (edge2.y * deltaUV1.x - edge1.y * deltaUV2.x);
        bitangent2.z = f * (edge2.z * deltaUV1.x - edge1.z * deltaUV2.x);
        bitangent2 = glm::normalize(bitangent2);

        float quadVertices[] = {
            // positions        // texture Coords  // normal      // tangent                       //bitangent
            pos1.x, pos1.y, pos1.z, uv1.x, uv1.y,   nm.x, nm.y, nm.z,  tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, uv2.x, uv2.y,   nm.x, nm.y, nm.z,  tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, uv3.x, uv3.y,   nm.x, nm.y, nm.z,  tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,


            pos1.x, pos1.y, pos1.z, uv1.x, uv1.y,   nm.x, nm.y, nm.z,  tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, uv3.x, uv3.y,   nm.x, nm.y, nm.z,  tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, uv4.x, uv4.y,   nm.x, nm.y, nm.z,  tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,

        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(5 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(11 * sizeof(float)));

    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// renders the 3D scene
// --------------------
void renderScene(const Shader &shader, unsigned int planeVAO)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
}


// renders the 3D scene
// --------------------
void renderScene3D(const Shader &shader)
{
    // room cube
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f));
    shader.setMat4("model", model);
    glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    renderCube();
    shader.setInt("reverse_normals", 0); // and of course disable it
    glEnable(GL_CULL_FACE);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    shader.setMat4("model", model);
    renderCube();
}

float lerp(float a,  float b,  float f) 
{
    return a + f * (b - a);
}
