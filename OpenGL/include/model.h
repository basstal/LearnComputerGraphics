#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <mesh.h>
#include <shader.h>
#include <vector>
#include <string>

unsigned int loadImage(const char * path, const std::string &);

class Model
{
public:
    Model(const char * path);
    void Draw(Shader shader);
private:
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode * node, const aiScene * scene);
    Mesh processMesh(aiMesh * mesh, const aiScene * scene);
    std::vector<Texture> loadMaterialTexture(aiMaterial * mat, aiTextureType type, std::string typeName);
};

Model::Model(const char * path)
{
    loadModel(path);
}

void Model::Draw(Shader shader)
{
    // std::cout << "meshes.size() = "  << meshes.size() << std::endl;
    for (unsigned int i = 0; i < meshes.size(); ++i)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate);
    // 这里需要判断scene是不是成功加载了
    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode( scene->mRootNode, scene );
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
    for (unsigned int i = 0 ; i < node->mNumMeshes; ++i )
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 顶点坐标、法线和纹理坐标
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        glm::vec3 position;
        position.x = mesh->mVertices[i].x;
        position.y = mesh->mVertices[i].y;
        position.z = mesh->mVertices[i].z;
        vertex.Position = position;
        
        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.Normal = normal;

        glm::vec2 texCoord(0.0f);
        if (mesh->mTextureCoords[0])
        {
            // 允许一个顶点包含最多8组纹理坐标，这里只用第一组
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
        }
        vertex.TexCoord = texCoord;
        vertices.push_back(vertex);
    }

    // 顶点索引属性
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // 材质（漫反射和镜面反射贴图）
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseTextures = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        // std::cout << "load material count = " << diffuseTextures.size() << std::endl;
        textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        std::vector<Texture> specularTextures = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
        // std::cout << "load material count = " << specularTextures.size() << std::endl;
        textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
    }
    else
    {
        bool skip = false;
        for (unsigned int i = 0 ; i < textures_loaded.size(); ++i)
        {
            if (textures_loaded[i].Type == "my_diffuse")
            {
                textures.push_back(textures_loaded[i]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            std::cout << "my orange loaded" << std::endl;

            // 自己张图贴上去
            Texture texture;
            texture.ID = loadImage("Orange.jpg", "");
            texture.Type = "my_diffuse";
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return Mesh(vertices, indices, textures);
}


std::vector<Texture> Model::loadMaterialTexture(aiMaterial * mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0 ; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); ++j)
        {
            if(strcmp(textures_loaded[j].Path.C_Str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.ID = loadImage(str.C_Str(), directory);
            texture.Type = typeName;
            texture.Path = str;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}


unsigned int loadImage(const char * path, const std::string &directory)
{
    std::string fileName = std::string(path);
    if (!directory.empty())
        fileName = directory + '/' + fileName;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrchannel;
    unsigned char * data = stbi_load(fileName.c_str(), &width, &height, &nrchannel, 0);
    if ( data != NULL )
    {
        GLint format;
        if (nrchannel == 1)
            format = GL_RED;
        else if (nrchannel == 3)
            format = GL_RGB;
        else if (nrchannel == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR::LOAD IMAGE::FAILED!" << std::endl;
    }
    
    stbi_image_free(data);
    return texture;
}

#endif