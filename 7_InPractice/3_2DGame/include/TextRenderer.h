#ifndef _TEXTRENDERER_H_
#define _TEXTRENDERER_H_

#include <Shader.h>
#include <string>
#include <map>
#include <tuple>
#include <Utils.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer
{
public:
    std::shared_ptr<Shader> shader;

    std::map<char, Character> Characters;
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));

    void Load(const char * inPath, unsigned int fontSize);

    TextRenderer(std::shared_ptr<Shader> inShader);
protected:
    unsigned int VBO;
    unsigned int planeVAO;
};

#endif