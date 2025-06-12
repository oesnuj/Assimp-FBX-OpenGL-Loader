// Material.cpp
#include "Material.h"
#include <iostream>

void Material::Bind(unsigned int shaderProgram) {
    // (����) diffuse �ؽ�ó�� ������ 0�� �ؽ�ó ���ֿ� ���ε�
    if (!diffuseTextures.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTextures[0].GetID());
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_diffuse1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "hasDiffuseTexture"), 1);
    }
    else {
        glUniform1i(glGetUniformLocation(shaderProgram, "hasDiffuseTexture"), 0);
    }
    // specular, normal � �����ϰԡ�
}

void Material::Unbind() {
    // ���ε� ����
    glBindTexture(GL_TEXTURE_2D, 0);
}
