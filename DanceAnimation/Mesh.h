// Mesh.h
#pragma once
#include <vector>
#include <glm/glm.hpp>

// 수정된 Vertex 구조체
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    glm::ivec4 BoneIDs = glm::ivec4(0);
    glm::vec4 Weights = glm::vec4(0.0f);
};


class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Draw();
    void PrintInfo() const;
};
