#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

// #define STB_IMAGE_IMPLEMENTATION
#include <others/stb_image.h>
#include <iostream>
#include <vector>
// #include <codecvt>
// #include <locale>

unsigned int LoadSkyboxTex(std::vector<std::string> skyboxTexs);


unsigned int loadImageGamma(const char * path, bool openGammaCorrection, bool flipOnLoad);

unsigned int loadImage(const char * path, bool flipOnLoad);

void replaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

void getProjectFilePath(const char * projectRelativePath2File, std::string& outputPath);


#endif