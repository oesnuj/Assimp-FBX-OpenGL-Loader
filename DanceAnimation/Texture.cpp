#include "Texture.h"
#include <iostream>

// SOIL ���̺귯�� ��� stb_image ��� (�� �Ϲ���)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Texture::Texture() : ID(0) {}
Texture::Texture(const std::string& filePath, const std::string& typeName)
    : type(typeName), path(filePath) {
    Generate(filePath);
}
Texture::~Texture() {}


void Texture::Bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Generate(const std::string& filePath) {
    glGenTextures(1, &ID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    printf("�ؽ�ó ����: %dx%d, ä��:%d\n", width, height, nrChannels);
    if (data) {
        printf("ù �ȼ� RGB: %d, %d, %d\n", data[0], data[1], data[2]);
    }
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        std::cout << "[�����] glTexImage2D - path: " << filePath
            << ", size: " << width << "x" << height
            << ", nrChannels: " << nrChannels << std::endl;

        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // �ؽ�ó �Ķ���� ����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Texture loaded: " << filePath << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    }
    else {
        std::cout << "Failed to load texture: " << filePath << std::endl;
        // �⺻ �ؽ�ó ���� (��� 1x1 �ȼ�)
        unsigned char whitePixel[3] = { 255, 255, 255 };
        glBindTexture(GL_TEXTURE_2D, ID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, whitePixel);
    }

    stbi_image_free(data);
}

unsigned int Texture::LoadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);


    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture at path: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}