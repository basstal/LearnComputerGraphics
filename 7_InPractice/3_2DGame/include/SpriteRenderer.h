#ifndef _SPRITERENDERER_H_
#define _SPRITERENDERER_H_

#include <Shader.h>
#include <Texture.h>

class SpriteRenderer
{
    public:
        SpriteRenderer(std::shared_ptr<Shader> inShader);
        ~SpriteRenderer();

        void DrawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 position, 
            glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, 
            glm::vec3 color = glm::vec3(1.0f));
        std::shared_ptr<Shader>       shader; 
    private:
        unsigned int quadVAO;

        void initRenderData();
};

#endif