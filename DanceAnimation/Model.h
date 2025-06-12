#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <glm/glm.hpp> 
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include <map>
#include "Bone.h"

class Animation;

class Model {
public:
    Model(const std::string& path);
    ~Model();
    void Draw(unsigned int shaderProgram);
    void PrintInfo();

    int AddBone(const std::string& boneName) {
        int id = boneCount++;
        boneNameToID[boneName] = id;
        return id;
    }
    int GetBoneID(const std::string& boneName) const {
        auto it = boneNameToID.find(boneName);
        return it == boneNameToID.end() ? -1 : it->second;
    }  
    
    const aiScene* GetScene() const { return scene; }


    std::map<std::string, int> boneNameToID;
    std::vector<glm::mat4> boneOffsetMatrices;
    Animation* GetAnimation() const { return animation; }
    glm::mat4 GetGlobalInverseTransform() const { return globalInverseTransform; }



private:
    std::vector<Mesh*> meshes;
    std::vector<Material> materials;
    std::vector<Texture> texturesLoaded;
    std::string directory;
    const aiScene* scene;
    int boneCount = 0;
    Animation* animation = nullptr;
    glm::mat4 globalInverseTransform;

    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
    Texture LoadEmbeddedTexture(const aiTexture* aiTex, const std::string& typeName);
};