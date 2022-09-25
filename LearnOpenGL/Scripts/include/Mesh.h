#ifndef _MESH_H_
#define _MESH_H_

#include <assimp/types.h>

#include <string>
#include <vector>

#include <Shader.h>

#include <glm/matrix.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

struct Texture
{
    unsigned int ID;
    std::string Type;
    aiString Path;
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(const Shader& shader);
    void BindTexture(const Shader& shader);
    unsigned int GetVAO();
private:
    // render data
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};


#endif