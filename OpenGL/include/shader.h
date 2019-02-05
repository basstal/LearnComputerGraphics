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

    Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

    void use();

    void setBool(const std::string & varName, bool val) const;
    void setInt(const std::string & varName, int val) const;
    void setFloat(const std::string & varName, float val) const;
    void setMat4(const std::string & varName, glm::mat4 mat4) const;
    void setVec3(const std::string &varName, glm::vec3 val) const;

};

#endif