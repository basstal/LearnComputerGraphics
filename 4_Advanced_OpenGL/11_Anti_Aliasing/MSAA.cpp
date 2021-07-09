#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>
#include <model.h>

#include <iostream>
#include <vector>
#include <map>

bool wireframe = false;


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

float quadVertices[] = {
    // position //TexCoords
    -1.0, -1.0, 0.0, 0.0,
    1.0, -1.0, 1.0, 0.0,
    -1.0, 1.0, 0.0, 1.0,
    -1.0, 1.0, 0.0, 1.0,
    1.0, -1.0, 1.0, 0.0,
    1.0, 1.0, 1.0, 1.0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int samples = 4;

unsigned int cubeVAO, cubeVBO;
unsigned int grassVAO, grassVBO;
unsigned int planeVAO, planeVBO;
unsigned int quadVAO, quadVBO;
unsigned int frameBuffer, multiSampleFBO, texMultisampleFBO;
unsigned int renderbufferMultisample, renderbufferMultisampleColor, renderbufferDepth;
unsigned int multisampleTextureAttachment;
unsigned int cubeTexture, floorTexture, grassTexture, windowTexture, framebufferTexture;
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
    glfwWindowHint(GLFW_SAMPLES, samples);
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

    glEnable(GL_DEPTH_TEST);

    Shader shader("../../Shaders/3_3/ModelVS33.vs", "../../Shaders/4_2/SimpleFragmentShader.fs", NULL);
    Shader backpackShader("../../Shaders/3_3/ModelVS33.vs", "../../Shaders/4_3/WindowFragmentShader.fs", NULL);
    Shader postProcessingShader("../../Shaders/4_5/FramebufferVS.vs", "../../Shaders/4_5/PostProcessing.fs", NULL);

    // backpack
    Model backpack("../../Assets/backpack/backpack.obj", true);

    // cube
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    // quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    // framebuffer with multisampled renderbuffer objects
    glGenFramebuffers(1, &multiSampleFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, multiSampleFBO);

    // use renderbuffer as color_attachment
    glGenRenderbuffers(1, &renderbufferMultisampleColor);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufferMultisampleColor);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbufferMultisampleColor);

    glGenRenderbuffers(1, &renderbufferMultisample);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufferMultisample);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferMultisample);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: multiSampleFBO Framebuffer is not complete!" << std::endl;
    }

    // framebuffer with multisampled texture attachment
    glGenFramebuffers(1, &texMultisampleFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, texMultisampleFBO);

    glGenTextures(1, &multisampleTextureAttachment);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampleTextureAttachment);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampleTextureAttachment, 0);

    // use the same depth stencil buffer as before
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferMultisample);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: texMultisampleFBO Framebuffer is not complete!" << std::endl;
    }


    // normal framebuffer with texture attachment
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

    glGenRenderbuffers(1, &renderbufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::FRAMEBUFFER:: frameBuffer Framebuffer is not complete!" << std::endl;
    }


    if (!wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glBindFramebuffer(GL_FRAMEBUFFER, texMultisampleFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0);
        shader.setMat4("model", model);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(5.0, 0.0, 0.0));
        backpackShader.use();
        backpackShader.setMat4("projection", projection);
        backpackShader.setMat4("view", view);
        backpackShader.setMat4("model", model);
        backpack.Draw(backpackShader);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, texMultisampleFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // do post-processing
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        postProcessingShader.use();
        // framebufferTexture is not a multisample texture
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

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