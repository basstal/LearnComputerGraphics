#ifndef _POSTPROCESSOR_H_
#define _POSTPROCESSOR_H_

#include <Shader.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class PostProcessor
{
public:

    std::shared_ptr<Shader> shader;
    PostProcessor(std::shared_ptr<Shader> shader);
    void PreDraw();
    void Draw();
    void ReloadTex();

    unsigned int framebuffer;
    bool bChaos = false;
    bool bConfuse = false;
    bool bShake = false;
protected:
    unsigned int framebufferMultisampled;
    unsigned int multisampledTex, postprocessedTex;
    unsigned int renderbuffer;
    unsigned int quadVAO;
};


#endif