#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int loadImage(const char * path, const std::string &directory, bool flipOnLoad)
{
    std::string fileName = std::string(path);
    if (!directory.empty())
        fileName = directory + '/' + fileName;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrchannel;
    stbi_set_flip_vertically_on_load(flipOnLoad);
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

#endif