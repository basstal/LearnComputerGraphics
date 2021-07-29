#include <Model.h>


Model::Model(const char * path, bool flipTexturesOnLoad)
{
    this->flipTexturesOnLoad = flipTexturesOnLoad;
    std::string absPath;
    getProjectFilePath(path, absPath);
    loadModel(absPath);
}

std::vector<Mesh> &Model::GetMeshes()
{
    return meshes;
}

void Model::Draw(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); ++i)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    /*
    aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
    
    aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes which is useful if your rendering has a maximum number of vertices allowed and can only process smaller meshes.
    
    aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into one larger mesh, reducing drawing calls for optimization.
    
    aiProcess_Triangulate: we tell Assimp that if the model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first.
    
    aiProcess_FlipUVs: flips the texture coordinates on the y-axis where necessary during processing.
    */
    const aiScene * scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
    // check scene is successfully loaded
    if ( !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    replaceStringInPlace(path, "\\", "/");
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

    // vertex position, normal position, texture coordinate
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
        
        glm::vec3 tangent;
        tangent.x = mesh->mTangents[i].x;
        tangent.y = mesh->mTangents[i].y;
        tangent.z = mesh->mTangents[i].z;
        vertex.Tangent = tangent;

        glm::vec2 texCoord(0.0f);
        if (mesh->mTextureCoords[0])
        {
            // allow 8 texture coordinates per vertex position, use only 1 here
            texCoord.x = mesh->mTextureCoords[0][i].x;
            texCoord.y = mesh->mTextureCoords[0][i].y;
        }
        vertex.TexCoords = texCoord;
        vertices.push_back(vertex);
    }

    // vertex index attribute
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // materials ( diffuse and reflection texture)
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseTextures = loadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
        std::vector<Texture> specularTextures = loadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
        std::vector<Texture> reflectionTexture = loadMaterialTexture(material, aiTextureType_AMBIENT, "texture_reflection");
        textures.insert(textures.end(), reflectionTexture.begin(), reflectionTexture.end());
        std::vector<Texture> tangentTexture = loadMaterialTexture(material, aiTextureType_HEIGHT, "texture_tangent");
        textures.insert(textures.end(), tangentTexture.begin(), tangentTexture.end());
    }
    else
    {
        bool skip = false;
        for (unsigned int i = 0 ; i < textures_loaded.size(); ++i)
        {
            if (textures_loaded[i].Type == "texture_diffuse")
            {
                textures.push_back(textures_loaded[i]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            std::cout << "awesomeface loaded" << std::endl;

            // map a texture by you self
            Texture texture;
            std::string path;
            getProjectFilePath("Assets/awesomeface.png", path);
            texture.ID = loadImage(path.c_str(), flipTexturesOnLoad);
            texture.Type = "texture_diffuse";
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
            // if texture hasn't been loaded already, load it
            Texture texture;
            if (!directory.empty())
            {
                std::string pathStr(directory);
                pathStr.append("/");
                pathStr.append(str.C_Str());
                texture.ID = loadImage(pathStr.c_str(), flipTexturesOnLoad);
            }
            else
            {
                texture.ID = loadImage(str.C_Str(), flipTexturesOnLoad);
            }
            texture.Type = typeName;
            texture.Path = str;
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}