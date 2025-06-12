#include "Mesh.h"
#include <GL/glew.h>
#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices)
{
    // OpenGL 버퍼 생성
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // BoneIDs (정수형 -> 정수 속성)
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneIDs));

    // Weights
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

    //for (int i = 0; i < 10; ++i) {
    //    const auto& v = vertices[i];
    //    std::cout << "[Vertex " << i << "] BoneIDs = "
    //        << v.BoneIDs.x << ", " << v.BoneIDs.y << ", "
    //        << v.BoneIDs.z << ", " << v.BoneIDs.w << " | "
    //        << "Weights = "
    //        << v.Weights.x << ", " << v.Weights.y << ", "
    //        << v.Weights.z << ", " << v.Weights.w << std::endl;
    //}

    glBindVertexArray(0);
}



Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::Draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::PrintInfo() const {
    std::cout << "[Mesh Info]" << std::endl;
    std::cout << " - Vertices: " << vertices.size() << std::endl;
    std::cout << " - Indices: " << indices.size() << std::endl;
    if (!vertices.empty()) {
        std::cout << " - First Vertex Pos: ("
            << vertices[0].Position.x << ", "
            << vertices[0].Position.y << ", "
            << vertices[0].Position.z << ")" << std::endl;
    }
}
