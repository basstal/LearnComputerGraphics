
#include <string>

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <Utils.h>

unsigned int LoadSkyboxTex(std::vector<std::string> skyboxTexs)
{
    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    int width, height, nrChannel;
    for (unsigned int i = 0 ; i < skyboxTexs.size(); ++i)
    {
        unsigned char * data = stbi_load(skyboxTexs[i].c_str(), &width, &height, &nrChannel, 0);
        GLint format;
        if (nrChannel == 1)
            format = GL_RED;
        else if (nrChannel == 3)
            format = GL_RGB;
        else if (nrChannel == 4)
            format = GL_RGBA;

        if ( data )
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "ERROR::LOAD SKYBOX TEXTURE FALIED" << std::endl;
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return skyboxTex;
}


unsigned int loadImageGamma(const char * path, bool openGammaCorrection, bool flipOnLoad)
{
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannel;
    stbi_set_flip_vertically_on_load(flipOnLoad);
    unsigned char * data = stbi_load(path, &width, &height, &nrChannel, 0);
    if (data != NULL)
    {
        GLint format;
        GLint internalFormat;

        if (nrChannel == 1)
            internalFormat = format = GL_RED;
        else if (nrChannel == 3)
        {
            format = GL_RGB;
            internalFormat = openGammaCorrection ? GL_SRGB : GL_RGB;
        }
        else if (nrChannel == 4)
        {
            internalFormat = openGammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;   
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "ERROR::LOAD IMAGE FAILED!" << std::endl;
    }
    stbi_image_free(data);


    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}

unsigned int loadImage(const char * path, bool flipOnLoad)
{
    std::string filePath;
    getProjectFilePath(path, filePath);
    // std::string fileName = std::string(path);
    // if (!directory.empty())
        // fileName = directory + '/' + fileName;
    // std::cout << "path : " << path << std::endl;
    // std::cout << "filePath : " << filePath << std::endl;
    
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrchannel;
    stbi_set_flip_vertically_on_load(flipOnLoad);
    unsigned char * data = stbi_load(filePath.c_str(), &width, &height, &nrchannel, 0);
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

void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
    if (search.empty())
    {
        return;
    }
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

void getProjectFilePath(const char * projectRelativePath2File, std::string& outputPath)
{
    std::string inPath(projectRelativePath2File);
    if (!inPath.empty() && inPath.find("LearnOpenGL") == -1)
    {
        wchar_t absolutePath[300];
        _wgetcwd(absolutePath, 300);
        // std::wcout<< absolutePath <<std::endl;
        std::wstring pathStr(absolutePath);
        // std::wcout << pathStr <<std::endl;
        std::wstring::size_type index = pathStr.rfind(L"LearnOpenGL");
        // std::cout<<"index : " << index <<std::endl;
        pathStr = pathStr.substr(0, index + 11);
        outputPath.clear();
        outputPath.append(pathStr.begin(), pathStr.end());
        outputPath.push_back('\\');
        outputPath.append(projectRelativePath2File);
        // std::cout << "outputPath : " << outputPath << std::endl;
        replaceStringInPlace(outputPath, "/", "\\");
    }
    else
    {
        outputPath = inPath;
    }
}
