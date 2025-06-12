#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "MatrixUtil.h"


std::string CleanNodeName(const std::string& name) {
    // "_end", "_$AssimpFbx$" 등 제거
    size_t end_pos = name.find("_end");
    if (end_pos != std::string::npos) {
        return name.substr(0, end_pos);
    }

    size_t assimp_pos = name.find("_$AssimpFbx$");
    if (assimp_pos != std::string::npos) {
        return name.substr(0, assimp_pos);
    }

    return name;
}


Animator::Animator(Animation* animation)
    : currentAnimation(animation), currentTime(0.0f), deltaTime(0.0f) {
    finalBoneMatrices.resize(100, glm::mat4(1.0f));

    // 💡 Hips 본의 global transform을 기준으로 inverse 잡기
    std::string hipsName = "mixamorig:Hips";
    const auto& rootNode = animation->GetRootNode();

    // 🔍 노드 이름이 정리된 상태일 수 있으므로 clean 해줌
    std::string cleanHips = CleanNodeName(hipsName);

    if (currentAnimation->HasBoneInfo(cleanHips)) {
        //std::cout << "[Animator] Using Hips bone for inverse transform\n";
        globalInverseTransform = glm::inverse(rootNode.transformation);
    }
    else {
        //std::cout << "[Animator] Fallback: using root node transform for inverse\n";
        globalInverseTransform = glm::inverse(rootNode.transformation);
    }
}

Animator::Animator(Animation* animation, const glm::mat4& globalInverse)
    : currentAnimation(animation), currentTime(0.0f), deltaTime(0.0f), globalInverseTransform(globalInverse)
{
    finalBoneMatrices.resize(100, glm::mat4(1.0f));
}


void Animator::UpdateAnimation(float deltaTime) {
    if (currentAnimation) {
        currentTime += currentAnimation->GetTicksPerSecond() * deltaTime;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());

        //std::cout << "[Animator::Update] CurrentTime=" << currentTime << std::endl;
        CalculateBoneTransform(currentAnimation->GetRootNode(), glm::mat4(1.0f));
        //std::cout << "[Animator::Update] BoneTransform 계산 완료" << std::endl;
    }
}

void Animator::CalculateBoneTransform(const AssimpNodeData& node, const glm::mat4& parentTransform) {
    //std::string nodeName = node.name;
    std::string nodeName = CleanNodeName(node.name);
    glm::mat4 nodeTransform = node.transformation;

    Bone* bone = currentAnimation->FindBone(nodeName);
    if (bone) {
        bone->Update(currentTime);
        nodeTransform = bone->GetLocalTransform();
    }
    else {
        //std::cout << "[Animator] Bone not found for node: " << nodeName << std::endl;
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    if (currentAnimation->HasBoneInfo(nodeName)) {
        int index = currentAnimation->GetBoneID(nodeName);
        glm::mat4 offset = currentAnimation->GetBoneOffset(nodeName);
        finalBoneMatrices[index] = globalInverseTransform * globalTransform * offset;
        //std::cout << "[Animator] Bone: " << nodeName
        //    << " | Final.x = " << finalBoneMatrices[index][3][0]
        //    << ", y = " << finalBoneMatrices[index][3][1]
        //    << ", z = " << finalBoneMatrices[index][3][2]
        //    << std::endl;
    }

    for (unsigned int i = 0; i < node.children.size(); i++) {
        CalculateBoneTransform(node.children[i], globalTransform);
    }
}


std::vector<glm::mat4> Animator::GetFinalBoneMatrices() const {
////    for (int i = 0; i < 10; ++i) {
////    std::cout << "Bone[" << i << "] matrix:\n";
////    const glm::mat4& m = finalBoneMatrices[i];
////    for (int r = 0; r < 4; ++r) {
////        std::cout << "[ ";
////        for (int c = 0; c < 4; ++c) {
////            std::cout << m[c][r] << " ";
////        }
////        std::cout << "]\n";
////    }
////}
    return finalBoneMatrices;
}