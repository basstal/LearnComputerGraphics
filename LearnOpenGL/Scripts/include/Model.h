#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <Mesh.h>
#include <Shader.h>
#include <vector>
#include <string>

#include <Utils.h>


class Model
{
public:
    Model(const char * path, bool flipTexturesOnLoad);
    void Draw(Shader &shader);
    std::vector<Mesh> &GetMeshes();
private:
    bool flipTexturesOnLoad = false;
    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode * node, const aiScene * scene);
    Mesh processMesh(aiMesh * mesh, const aiScene * scene);
    std::vector<Texture> loadMaterialTexture(aiMaterial * mat, aiTextureType type, std::string typeName);
};



#endif