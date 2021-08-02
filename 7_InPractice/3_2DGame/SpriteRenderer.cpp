#include <SpriteRenderer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void SpriteRenderer::initRenderData()
{
    glGenVertexArrays(1, &quadVAO);
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenBuffers(1, &VBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void SpriteRenderer::DrawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 position, 
            glm::vec2 size, float rotate, 
            glm::vec3 color)
{
    shader->use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->TextureID);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));

    model = glm::translate(model, glm::vec3(size.x / 2, size.y / 2, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0, 0, 1.0));
    model = glm::translate(model, glm::vec3(-size.x / 2, -size.y / 2, 0.0f));

    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0));

    shader->setMat4("model", model);
    shader->setVec3("spriteColor", color);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

SpriteRenderer::SpriteRenderer(std::shared_ptr<Shader> inShader)
{
    shader = inShader;
    initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{

}