#include "ResourceManager.h"
#include <Shader.h>
#include <iostream>
#include <exception>
#include <Utils.h>

using namespace std;

std::map<std::string, shared_ptr<Shader>> ResourceManager::Shaders;
std::map<std::string, shared_ptr<Texture2D>> ResourceManager::Textures;

class loadexception: public exception
{
    virtual const char* what() const throw()
    {
        return "ResourceManager get resource error";
    }
} loadex;

shared_ptr<Shader>    ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name)
{
    if (Shaders.find(name) == Shaders.end())
    {
        shared_ptr<Shader> shader = make_shared<Shader>(vShaderFile, fShaderFile, gShaderFile);
        Shaders.insert(std::pair<std::string, shared_ptr<Shader>>(name, shader));
        return shader;
    }
    else
    {
        return (*Shaders.find(name)).second;
    }
}

shared_ptr<Shader>   ResourceManager::GetShader(std::string name)
{
    if (Shaders.find(name) == Shaders.end())
    {
        throw loadex;
    }
    return (*Shaders.find(name)).second;
}

shared_ptr<Texture2D> ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    if (Textures.find(name) == Textures.end())
    {
        unsigned int texture2d = loadImage(file, false);
        shared_ptr<Texture2D> tex = make_shared<Texture2D>();
        tex->TextureID = texture2d;
        Textures.insert(pair<string, shared_ptr<Texture2D>>(name, tex));
        return tex;
    }
    else
    {
        return (*Textures.find(name)).second;
    }
}

shared_ptr<Texture2D> ResourceManager::GetTexture(std::string name)
{
    if (Textures.find(name) == Textures.end())
    {
        throw loadex;
    }
    return (*Textures.find(name)).second;
}

void      ResourceManager::Clear()
{
    Textures.clear();
    Shaders.clear();
}