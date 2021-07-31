#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <others/stb_image.h>

#include <iostream>
#include <vector>
#include <random>

#include <Camera.h>
#include <Shader.h>
#include <Model.h>

#include <others/Icosphere.h>

#include <map>

using namespace std;

static int drawMode = 0;
static std::map<int, const char *> drawModeMap = {
    {0, "Fill"},
    {1, "Wireframe"},
    {2, "Point"},
};

static int HEIGHT = 1080;
static int WIDTH = 1920;

static bool firstMove = true;
static float lastX = 0.0f;
static float lastY = 0.0f;

static float lastFrame = 0.0f;
static float deltaTime = 0.0f;

static Camera camera = Camera(glm::vec3(0.0, 0.0, 4.0), glm::vec3(0, 1, 0), -124, -19);
static std::shared_ptr<Shader> deferredShader, wireframeShader, pointShader, shaderSSAO;
static std::shared_ptr<Shader> debugQuadShader;
static std::shared_ptr<Shader> quadShader;
static std::shared_ptr<Shader> lightShader;
static std::shared_ptr<Model> backpack = nullptr;
static unsigned int gBufferFBO;
static std::vector<glm::vec3> objectPositions;
static unsigned int gPosition, gNormal, gAlbedoSpec, noiseTexture;
static std::vector<glm::vec3> lightPositions;
static std::vector<glm::vec3> lightColors;
static vector<glm::vec3> ssaoKernel;
static std::vector<float> lightRadius;
static float constant  = 1.0, linear    = 0.7, quadratic = 1.8; 
static const unsigned int NR_LIGHTS = 32;
static unsigned int depthRB, ssaoFBO, ssaoColorBuffer, ssaoBlurFBO, ssaoColorBufferBlur;
static unsigned int ubo;

static void frame_buffer_callback(GLFWwindow *, int , int);
static void cursor_pos_callback(GLFWwindow *, double, double);
static void mouse_scroll_callback(GLFWwindow *, double, double);
static void processInput(GLFWwindow * window);
static void renderCube();
static void renderQuad();
static void renderScene(const Shader &shader, unsigned int planeVAO);
static void renderScene3D(const Shader &shader);
static void renderQuadSimple();
static void renderCubeSimple();
static float lerp(float, float, float);
static void calculateLightInfo(GLFWwindow * window, bool);
// static void drawImGuiContent(GLFWwindow * window);

static bool bCursorOff = false;
static bool bPressed;

static void switch_cursor(GLFWwindow * window)
{
    if (!bCursorOff)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        firstMove = true;
    }
    bCursorOff = !bCursorOff;
}

static void RecompileShaders(GLFWwindow * window)
{
    deferredShader->recompileFromSource();
    wireframeShader->recompileFromSource();
    pointShader->recompileFromSource();
    debugQuadShader->recompileFromSource();
    quadShader->recompileFromSource();
    lightShader->recompileFromSource();
    shaderSSAO->recompileFromSource();

    shaderSSAO->use();
    shaderSSAO->setInt("gPosition", 0);
    shaderSSAO->setInt("gNormal", 1);
    shaderSSAO->setInt("texNoise", 2);

    quadShader->use();
    quadShader->setInt("gPosition", 0);
    quadShader->setInt("gNormal", 1);
    quadShader->setInt("gAlbedoSpec", 2);
    quadShader->setInt("ssaoColorBuffer", 3);

    wireframeShader->use();
    unsigned int blockIndex = glGetUniformBlockIndex(wireframeShader->ID, "Matrices");
    glUniformBlockBinding(wireframeShader->ID, blockIndex, 0);
}

static void SendKernelSamplesToShader()
{
    shaderSSAO->setFloat("radius", 0.5);
    shaderSSAO->setFloat("bias", 0.025);

    for (unsigned int i = 0 ; i < 64 ; ++ i)
    {
        shaderSSAO->setVec3("samples[" + to_string(i) + "]", ssaoKernel[i]);
    }
}
// static void switch_drawMode(GLFWwindow * window)
// {
//     // deferredShader->setInt("drawMode", drawMode);
//     // debugQuadShader->setInt("drawMode", drawMode);
//     // quadShader->setInt("drawMode", drawMode);
//     // lightShader->setInt("drawMode", drawMode);
//     // ** use custom shader
//     switch(drawMode)
//     {
//         case 0:
//             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//             break;
//         case 1:
//             glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//             break;
//         case 2:
//             glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//             break;
//         default:
//             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//             break;
//     }

// }



void SSAO_setup(GLFWwindow * window)
{
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//     GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "DeferredShading", NULL, NULL);
//     if (window == NULL)
//     {
//         cout << "ERROR::CREATE WINDOW:: FAILED!" << endl;
//         glfwTerminate();
//         return -1;
//     }

//     glfwMakeContextCurrent(window);

//     if( ! gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) )
//     {
//         cout << "ERROR::GLAD LOADER INIT FAILED!" <<endl;
//         glfwTerminate();
//         return -1;
//     }

    
    glEnable(GL_DEPTH_TEST);
    
    glfwSetFramebufferSizeCallback(window, frame_buffer_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwGetWindowSize(window, &WIDTH, &HEIGHT);

    // glfwSetCursorPosCallback(window, cursor_pos_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!backpack)
    {
        backpack = std::make_shared<Model>("Assets/backpack/backpack.obj", true);
    }

    
    
    deferredShader = std::make_shared<Shader>("Shaders/5_10/GBufferVS.vert", "Shaders/5_10/GBufferFS.frag", nullptr);
    wireframeShader = std::make_shared<Shader>("Shaders/5_9/Wireframe.vert", "Shaders/5_9/Wireframe.frag", "Shaders/5_9/Wireframe.geom");
    pointShader = std::make_shared<Shader>("Shaders/5_9/Point.vert", "Shaders/5_9/Wireframe.frag", "Shaders/5_9/Point.geom");
    // use this shader to debug G-Buffer
    debugQuadShader = std::make_shared<Shader>("Shaders/5_9/DebugQuadVS.vert", "Shaders/5_9/DebugQuadFS.frag", nullptr);
    quadShader = std::make_shared<Shader>("Shaders/5_9/DebugQuadVS.vert", "Shaders/5_10/QuadFS.frag", nullptr);
    lightShader = std::make_shared<Shader>("Shaders/2_3/MaterialsVS23.vert", "Shaders/2_3/ExerciseLight23.frag", nullptr);
    shaderSSAO = std::make_shared<Shader>("Shaders/5_10/SSAO.vert", "Shaders/5_10/SSAO.frag", nullptr);

    Icosphere sphere(1.0f, 3, false);    // radius, subdivision, smooth

    glGenFramebuffers(1, &gBufferFBO);
    glGenFramebuffers(1, &ssaoFBO);

    // position color buffer
    glGenTextures(1, &gPosition);
    // normal color buffer
    glGenTextures(1, &gNormal);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);

    glGenTextures(1, &noiseTexture);
    glGenTextures(1, &ssaoColorBuffer);

    

    glGenRenderbuffers(1, &depthRB);

    glGenFramebuffers(1, &ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    frame_buffer_callback(window, WIDTH, HEIGHT);



    
    // objectPositions.clear();
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0, -3.0));
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0,  0.0));
    // objectPositions.push_back(glm::vec3(-3.0,  -3.0,  3.0));
    // objectPositions.push_back(glm::vec3( 0.0,  -3.0,  3.0));
    // objectPositions.push_back(glm::vec3( 3.0,  -3.0,  3.0));

    ssaoKernel.clear();
    uniform_real_distribution<float> randomFloats(0.0, 1.0);
    default_random_engine generator;
    for(unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = (float) i / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoKernel.push_back(sample);

    }

    vector<glm::vec3> ssaoNoise;
    for(unsigned int i = 0; i < 16; ++i)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f
        );
        ssaoNoise.push_back(noise);
    }

    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(glm::mat4));
    
    
    
    // lighting info
    // -------------
    RecompileShaders(window);
    calculateLightInfo(window, false);
    // // Setup Dear ImGui context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // 

    // // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    

    // // Setup Platform/Renderer bindings
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init("#version 330 core");


    // while(!glfwWindowShouldClose(window))
    // {
        

    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //     glfwSwapBuffers(window);
    //     glfwPollEvents();
    // }

    // glfwTerminate();
}

void SSAO_imgui(GLFWwindow * window)
{
    // ImGui::Begin("Editor");
    // ImGui::Text("Lights Color & Position");
    // for (int i = 0; i < 4; ++i)
    // {
    //     ImGui::ColorEdit3(("light" + std::to_string(i) + " Color").c_str(), glm::value_ptr(lightColors[i]), ImGuiColorEditFlags_HDR);
    //     ImGui::DragFloat3(("light" + std::to_string(i) + " Position").c_str(), glm::value_ptr(lightPositions[i]), 0.05f, -88.f, 88.0f);
    //     ImGui::Separator();
    // }
    ImGui::Spacing();
    ImGui::Text("Light Info");
    bool bLightInfoChanged = false;
    bLightInfoChanged |= ImGui::SliderFloat("constant", &constant, 0, 20.f);
    bLightInfoChanged |= ImGui::SliderFloat("linear", &linear, 0.01f, 5.f);
    bLightInfoChanged |= ImGui::SliderFloat("quadratic", &quadratic, 0.01f, 3.f);
    if (bLightInfoChanged)
    {
        calculateLightInfo(window, true);
    }
    // ImGui::SliderFloat("shininess", &shininess, 0, 256.f);
    // ImGui::SliderFloat("ambientStrength", &ambientStrength, 0, 32.f);
    // ImGui::SliderFloat("exposure", &exposure, 0, 128.f);
    ImGui::Separator();
    if (bCursorOff)
    {
        ImGui::Text("Press P to release control of the camera, and show cursor.");
    }
    else
    {
        ImGui::Text("Press P or Button follows to take control of the camera");
        if(ImGui::Button("Posses camera") && !bCursorOff)
        {
            switch_cursor(window);
        }
    }
    if (ImGui::Button("Recompile Shaders"))
    {
        RecompileShaders(window);
    }
    ImGui::Separator();
    ImGui::Text("Current Draw Mode : %s", drawModeMap[drawMode]);
    if (ImGui::CollapsingHeader("Draw Mode Selection"))
    {
        for (int i = 0; i < 3; ++i)
        {
            ImGui::RadioButton(drawModeMap[i], &drawMode, i);
            if (i < 2)
            {
                ImGui::SameLine();
            }
        }
    }
    glm::vec3 pos = camera.Position;
    ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);

    // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    // ImGui::Text("Press space key to navigating in scene");
    // ImGui::End();
}

int SSAO(GLFWwindow * window)
{
    processInput(window);
    // switch_drawMode(window);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH/(float)HEIGHT, 0.1f, 100.f);
    glm::mat4 view = camera.GetViewMatrix();
    
    if (drawMode == 0)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        deferredShader->use();
        deferredShader->setMat4("projection", projection);
        deferredShader->setMat4("view", view);
        // for (unsigned int i = 0 ; i < objectPositions.size(); ++i)
        // {
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, objectPositions[i]);
        // model = glm::scale(model, glm::vec3(0.5f));
        deferredShader->setMat4("model", model);
        backpack->Draw(*deferredShader);
        model = glm::scale(model, glm::vec3(8));
        deferredShader->setMat4("model", model);
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        shaderSSAO->use();
        SendKernelSamplesToShader();
        shaderSSAO->setMat4("projection", projection);
        renderQuadSimple();


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);
        quadShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        quadShader->setMat4("model", glm::mat4(1));
        quadShader->setVec3("viewPos", camera.Position);
        quadShader->setFloat("shininess", 64);
        for (unsigned int i = 0 ; i < lightPositions.size(); ++i)
        {
            quadShader->setVec3("lights[" + to_string(i) + "].Position", lightPositions[i]);
            quadShader->setVec3("lights[" + to_string(i) + "].Color", lightColors[i]);
            quadShader->setFloat("lights[" + to_string(i) + "].Radius", lightRadius[i]);
            quadShader->setFloat("lights[" + to_string(i) + "].Linear", linear);
            quadShader->setFloat("lights[" + to_string(i) + "].Quadratic", quadratic);
        }
        renderQuadSimple();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        lightShader->use();
        lightShader->setMat4("projection", projection);
        lightShader->setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(lightPositions[i]));
            model = glm::scale(model, glm::vec3(0.1f));
            lightShader->setMat4("model", model);
            lightShader->setVec3("lightColor", lightColors[i]);
            renderCubeSimple();
        }
    }
    else 
    {
        // ** TODO:
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);
        // std::shared_ptr<Shader> usedShader = drawMode == 1 ? wireframeShader : pointShader;
        // usedShader->use();
        // usedShader->setVec3("color", glm::vec3(0, 0.496, 0.496));
        // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        // for (unsigned int i = 0 ; i < objectPositions.size(); ++i)
        // {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, objectPositions[i]);
        //     model = glm::scale(model, glm::vec3(0.5f));
        //     glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
        //     backpack->Draw(*usedShader);
        // }
        // for (unsigned int i = 0; i < lightPositions.size(); i++)
        // {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, glm::vec3(lightPositions[i]));
        //     model = glm::scale(model, glm::vec3(0.1f));
        //     glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));
        //     renderCubeSimple();
        // }
    }
    

    // // debug G-Buffer start
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // debugquadShader->use();
    // debugquadShader->setInt("renderTexture", 0);
    // glClearColor(1, 1, 1, 1.0f);
    // glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    // glm::mat4 model = glm::mat4(1.0);
    // model = glm::translate(model, glm::vec3(-0.5f, 0.5f, 0));
    // model = glm::scale(model, glm::vec3(0.495));
    // debugquadShader->setMat4("model", model);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, gPosition);
    // renderQuadSimple();

    // model = glm::mat4(1.0);
    // model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0));
    // model = glm::scale(model, glm::vec3(0.495));
    // debugquadShader->setMat4("model", model);
    // glBindTexture(GL_TEXTURE_2D, gNormal);
    // renderQuadSimple();

    // model = glm::mat4(1.0);
    // model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0));
    // model = glm::scale(model, glm::vec3(0.495));
    // debugquadShader->setMat4("model", model);
    // glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    // renderQuadSimple();

    // model = glm::mat4(1.0);
    // model = glm::translate(model, glm::vec3(0.5f, -0.5f, 0));
    // model = glm::scale(model, glm::vec3(0.495));
    // debugquadShader->setMat4("model", model);
    // debugquadShader->setBool("onlyAlpha", true);
    // glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    // renderQuadSimple();
    // debugquadShader->setBool("onlyAlpha", false);
    // // debug G-Buffer end

    
    return 0;

}

static void calculateLightInfo(GLFWwindow * window, bool fixedPosition)
{
    lightRadius.clear();
    if (!fixedPosition)
    {
        lightPositions.clear();
        lightColors.clear();
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
            float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
            // also calculate random color
            float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
            float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0

            glm::vec3 lightColor = glm::vec3(rColor, gColor, bColor);
            lightColors.push_back(lightColor);
        }
    }
    // srand(13);

    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {

        glm::vec3 lightColor = lightColors[i];
        float lightMax  = std::fmaxf(std::fmaxf(lightColor.r, lightColor.g), lightColor.b);
        float radius    = 
        (-linear +  std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * lightMax))) 
        / (2 * quadratic);  
        lightRadius.push_back(radius);
    }
}

static void processInput(GLFWwindow * window)
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

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
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && bPressed)
    {
        bPressed = false;
        switch_cursor(window);
    }
}

static void frame_buffer_callback(GLFWwindow *window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        WIDTH = width;
        HEIGHT = height;
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "ERROR::FRAME BUFFER INIT FAILED!" <<endl;
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
        glDrawBuffers(3, attachments);

        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, WIDTH, HEIGHT);
    }
}


static void cursor_pos_callback(GLFWwindow * window, double xPos, double yPos)
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

static void mouse_scroll_callback(GLFWwindow * window, double offsetX, double offsetY)
{
    camera.ProcessMouseScroll((float)offsetY);
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
static unsigned int cubeVAO = 0;
static unsigned int cubeVBO = 0;

static void renderCubeSimple()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
    }
    float vertices[] = {
        // back face
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        // left face
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        // right face
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
        // bottom face
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
        // top face
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-left        
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    };
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // render Cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

static void renderCube()
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
static unsigned int quadVAO = 0;
static unsigned int quadVBO;

static void renderQuadSimple()
{
    if (quadVAO == 0)
    {
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
    }
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

static void renderQuad()
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
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
static void renderScene(const Shader &shader, unsigned int planeVAO)
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
static void renderScene3D(const Shader &shader)
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

static float lerp(float a,  float b,  float f) 
{
    return a + f * (b - a);
}
