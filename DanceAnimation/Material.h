#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Texture.h"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    std::vector<Texture> diffuseTextures;
    std::vector<Texture> specularTextures;
    std::vector<Texture> normalTextures;
    
    Material() {
        ambient = glm::vec3(0.2f);
        diffuse = glm::vec3(0.8f);
        specular = glm::vec3(0.5f);
        shininess = 32.0f;
    }
    
    void Bind(unsigned int shaderProgram);
    void Unbind();
};