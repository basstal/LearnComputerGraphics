#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader(const Shader& shader);
    Shader(const char * vShaderCode, const char *fShaderCode, const char * gShaderCode, bool isSource);
    Shader(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath);
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath);

    void use();

    void recompileFromSource();

    void setBool(const std::string & varName, bool val) const;
    void setInt(const std::string & varName, int val) const;
    void setFloat(const std::string & varName, float val) const;
    void setMat4(const std::string & varName, glm::mat4 mat4) const;
    void setVec3(const std::string &varName, float *, int count) const;
    void setVec3(const std::string &varName, glm::vec3 val) const;
    void setVec3(const std::string &varName, float x, float y, float z) const;
    void setVec2(const std::string &varName, glm::vec2 val) const;
    void setVec4(const std::string &varName, glm::vec4 val) const;

protected:
    std::string vsPath;
    std::string fsPath;
    std::string gsPath;

    void compileAndLinkShaders(const char * vShaderCode, const char *fShaderCode, const char * gShaderCode);
    void readAndCompileAll(const GLchar * vertexPath, const GLchar * fragmentPath, const GLchar * geometryPath);
    std::string readShaderCode(const char *shaderPath);
};

#endif