#pragma once
#include <assimp/scene.h>
#include <string>
#include <unordered_map>
#include "Bone.h"
#include "Model.h"
#include <vector>           // <-- Ãß°¡
#include <glm/glm.hpp> 

struct AssimpNodeData {
    std::string name;
    glm::mat4 transformation;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation(const std::string& animationPath, class Model* model);
    Animation(const aiScene* scene, Model* model);
    Animation(const aiAnimation* animation, Model* model);
    Bone* FindBone(const std::string& name);

    inline float GetTicksPerSecond() const { return ticksPerSecond; }
    inline float GetDuration() const { return duration; }
    inline const AssimpNodeData& GetRootNode() const { return rootNode; }
    inline const std::unordered_map<std::string, Bone>& GetBoneMap() const { return bonesMap; }

    const glm::mat4& GetBoneOffset(const std::string& boneName) const;
    int GetBoneID(const std::string& boneName) const;
    bool HasBoneInfo(const std::string& boneName) const;

private:
    float duration;
    float ticksPerSecond;
    std::unordered_map<std::string, Bone> bonesMap;
    Model* modelPtr;

    AssimpNodeData rootNode;
    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
    void ReadMissingBones(const aiAnimation* animation, class Model& model);
};