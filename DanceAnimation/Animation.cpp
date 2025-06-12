#include "Animation.h"
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "MatrixUtil.h"



Animation::Animation(const std::string& animationPath, Model* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights);

    if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mNumAnimations == 0) {
        std::cerr << "[Animation Load Error] " << importer.GetErrorString() << std::endl;
        return;
    }

    aiAnimation* animation = scene->mAnimations[0];
    duration = static_cast<float>(animation->mDuration);
    ticksPerSecond = static_cast<float>(
        animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0f
        );


    ReadHierarchyData(rootNode, scene->mRootNode);  // ✅ 제대로 된 계층 구조 복사
    ReadMissingBones(animation, *model);

   // std::cout << "[Animation 생성 완료] Duration: " << duration
     //   << ", TPS: " << ticksPerSecond << std::endl;
}



Animation::Animation(const aiScene* scene, Model* model) : modelPtr(model) {
    if (!scene || scene->mNumAnimations == 0) {
        std::cerr << "[Animation Load Error] 유효하지 않은 scene 또는 애니메이션 없음" << std::endl;
        return;
    }

    aiAnimation* animation = scene->mAnimations[0];
    duration = static_cast<float>(animation->mDuration);
    ticksPerSecond = static_cast<float>(
        animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0f
        );

    ReadHierarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);

    //std::cout << "[Animation 생성 완료] Duration: " << duration
        //<< ", TPS: " << ticksPerSecond << std::endl;
}

// Animation.cpp 에

Animation::Animation(const aiAnimation* animation, Model* model)
    : modelPtr(model),
    duration(float(animation->mDuration)),
    ticksPerSecond(animation->mTicksPerSecond != 0.0f
        ? float(animation->mTicksPerSecond)
        : 60.0f)    // 기본 fps
{
    // scene 대신 받은 animation 하나로 본 정보 채우기
    ReadMissingBones(animation, *model);
    // Model 에서 가져올 루트 노드(AssimpNodeData) 세팅 코드가
    // 기존 scene 생성자( aiScene* )에 들어있다면
    // 그 코드를 여기에도 복사해 넣으시면 됩니다.
}



Bone* Animation::FindBone(const std::string& name) {
    auto it = bonesMap.find(name);
    if (it != bonesMap.end()) return &it->second;
    return nullptr;
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model) {
    int boneCount = 0;

    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        int boneID = model.GetBoneID(boneName);
        if (boneID == -1) {
            boneID = model.AddBone(boneName);
        }

        bonesMap[boneName] = Bone(boneName, boneID, channel);
        //std::cout << "[Animation] Bone loaded: " << boneName << " (ID=" << boneID << ")" << std::endl;
        boneCount++;
    }
    //std::cout << "[Animation] Total Bones in Animation: " << boneCount << std::endl;
}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    dest.name = std::string(src->mName.C_Str());
    dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation); // glm::mat4로 변환
    dest.children.resize(src->mNumChildren);

    for (unsigned int i = 0; i < src->mNumChildren; i++) {
        ReadHierarchyData(dest.children[i], src->mChildren[i]);
    }
}


const glm::mat4& Animation::GetBoneOffset(const std::string& boneName) const {
    //return bonesMap.at(boneName).GetLocalTransform();

    int boneID = modelPtr->GetBoneID(boneName);
    if (boneID >= 0 && boneID < modelPtr->boneOffsetMatrices.size()) {
        return modelPtr->boneOffsetMatrices[boneID];
    }
    static glm::mat4 identity(1.0f);
    return identity;
}

int Animation::GetBoneID(const std::string& boneName) const {
    return bonesMap.at(boneName).GetBoneID();
}

bool Animation::HasBoneInfo(const std::string& boneName) const {
    return bonesMap.find(boneName) != bonesMap.end();
}
