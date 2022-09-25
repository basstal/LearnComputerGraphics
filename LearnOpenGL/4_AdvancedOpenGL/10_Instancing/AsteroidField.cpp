#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <Camera.h>
#include <Shader.h>
#include <Model.h>

static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow *window);

// settings
extern int WIDTH, HEIGHT;

// camera
static Camera camera(glm::vec3(0, 0.0f, 60.0f));
static float lastX = (float)WIDTH  / 2.0;
static float lastY = (float)HEIGHT / 2.0;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

static std::shared_ptr<Shader> shaderProgram;
static std::shared_ptr<Shader> instanceAsteroidShader;

static std::shared_ptr<Model> planet = nullptr;
static std::shared_ptr<Model> rock = nullptr;

static unsigned int amount = 100000;
static glm::mat4 *modelMatrices = nullptr;


static bool bCursorOff = false;
static bool bPressed;

static void switch_cursor(GLFWwindow * window)
{
    if (!bCursorOff)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        firstMouse = true;
    }
    bCursorOff = !bCursorOff;
}

void asteroidField_setup(GLFWwindow * window)
{
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    if (!modelMatrices)
    {
        modelMatrices = new glm::mat4[amount];
        srand(glfwGetTime()); // initialize random seed	
        float radius = 150.0f;
        float offset = 25.0f;
        for(unsigned int i = 0; i < amount; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            // 1. translation: displace along circle with 'radius' in range [-offset, offset]
            float angle = (float)i / (float)amount * 360.0f;
            float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
            displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
            float z = cos(angle) * radius + displacement;
            model = glm::translate(model, glm::vec3(x, y, z));

            // 2. scale: scale between 0.05 and 0.25f
            float scale = (rand() % 20) / 100.0f + 0.05;
            model = glm::scale(model, glm::vec3(scale));

            // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

            // 4. now add to list of matrices
            modelMatrices[i] = model;
        }
    }


    shaderProgram = std::make_shared<Shader>("Shaders/4_10/AsteroidFieldVS.vert", "Shaders/4_10/AsteroidFieldFS.frag", nullptr);
    instanceAsteroidShader = std::make_shared<Shader>("Shaders/4_10/InstanceAsteroidVS.vert", "Shaders/4_10/AsteroidFieldFS.frag", nullptr);
    if (!planet)
    {

        planet = std::make_shared<Model>("Assets/planet/planet.obj", false);
    }
    if (!rock)
    {
        rock = std::make_shared<Model>("Assets/rock/rock.obj", false);

    }

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    std::vector<Mesh> meshes = rock->GetMeshes();
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        unsigned int VAO = meshes[i].GetVAO();
        glBindVertexArray(VAO);
        
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size * 2));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size * 3));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
}

void asteroidField_imgui(GLFWwindow * window)
{
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
    glm::vec3 pos = camera.Position;
    ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
    ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", camera.Yaw, camera.Pitch);
}

int asteroidField(GLFWwindow * window)
{
    processInput(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), WIDTH/ (float)HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shaderProgram->setMat4("projection", projection);
    shaderProgram->setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0));
    shaderProgram->setMat4("model", model);
    planet->Draw(*shaderProgram);

    // use instancing
    instanceAsteroidShader->use();
    instanceAsteroidShader->setMat4("projection", projection);
    instanceAsteroidShader->setMat4("view", view);
    std::vector<Mesh> meshes = rock->GetMeshes();
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        Mesh mesh = meshes[i];
        mesh.BindTexture(*instanceAsteroidShader);
        glBindVertexArray(mesh.GetVAO());
        glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);
    }
    
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
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