#include <PostProcessor.h>

extern int WIDTH, HEIGHT;

PostProcessor::PostProcessor(std::shared_ptr<Shader> inShader)
{
    shader = inShader;
    shader->use();
    

    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        {-offset, offset},
        {0.0f, offset},
        {offset, offset},
        {-offset, 0.0f},
        {0.0f, 0.0f},
        {offset, 0.0f},
        {-offset, -offset},
        {0.0f, -offset},
        {offset, -offset},
    };
    glUniform2fv(glGetUniformLocation(shader->ID, "offsets"), 9, (float *)offsets);
    float blurKernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    glUniform1fv(glGetUniformLocation(shader->ID, "blur_kernel"), 9, (float *)blurKernel);
    int edgeKernel[9] = {
        -1, -1, -1,
        -1, 8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(shader->ID, "edge_kernel"), 9, (int *)edgeKernel);




    glGenFramebuffers(1, &framebufferMultisampled);
    glGenFramebuffers(1, &framebuffer);
    ReloadTex();

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    unsigned int quadVBO;
    float quad[6][4] = {
        {-1.0f, -1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f, 0.0f},
        {-1.0f, 1.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
    };
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::ReloadTex()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferMultisampled);

    glDeleteTextures(1, &multisampledTex);
    glGenTextures(1, &multisampledTex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisampledTex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, WIDTH, HEIGHT, true);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisampledTex, 0);

    glDeleteRenderbuffers(1, &renderbuffer);
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        std::cout << "framebufferMultisampled status is not complete : " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glDeleteTextures(1, &postprocessedTex);
    glGenTextures(1, &postprocessedTex);
    glBindTexture(GL_TEXTURE_2D, postprocessedTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessedTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        std::cout << "framebuffer status is not complete : " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::PreDraw()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferMultisampled);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::Draw()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMultisampled);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    shader->use();
    shader->setFloat("time", (float)glfwGetTime());
    shader->setBool("chaos", bChaos);
    shader->setBool("confuse", bConfuse);
    shader->setBool("shake", bShake);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postprocessedTex);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

