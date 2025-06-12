// Material.cpp
#include "Material.h"
#include <iostream>

void Material::Bind(unsigned int shaderProgram) {
    // (예시) diffuse 텍스처가 있으면 0번 텍스처 유닛에 바인딩
    if (!diffuseTextures.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTextures[0].GetID());
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "hasDiffuseTexture"), 1);
    }
    else {
        glUniform1i(glGetUniformLocation(shaderProgram, "hasDiffuseTexture"), 0);
    }
    // specular, normal 등도 동일하게…
}

void Material::Unbind() {
    // 바인딩 해제
    glBindTexture(GL_TEXTURE_2D, 0);
}
