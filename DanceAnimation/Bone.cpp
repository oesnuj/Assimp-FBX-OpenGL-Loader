#define GLM_ENABLE_EXPERIMENTAL
#include "Bone.h"
#include <iostream>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : mName(name), mID(ID), mLocalTransform(1.0f)
{
    for (unsigned int i = 0; i < channel->mNumPositionKeys; i++) {
        aiVector3D pos = channel->mPositionKeys[i].mValue;
        float time = channel->mPositionKeys[i].mTime;
        mPositions.push_back({ glm::vec3(pos.x, pos.y, pos.z), time });
    }
    for (unsigned int i = 0; i < channel->mNumRotationKeys; i++) {
        aiQuaternion rot = channel->mRotationKeys[i].mValue;
        float time = channel->mRotationKeys[i].mTime;
        mRotations.push_back({ glm::quat(rot.w, rot.x, rot.y, rot.z), time });
    }
    for (unsigned int i = 0; i < channel->mNumScalingKeys; i++) {
        aiVector3D scale = channel->mScalingKeys[i].mValue;
        float time = channel->mScalingKeys[i].mTime;
        mScales.push_back({ glm::vec3(scale.x, scale.y, scale.z), time });
    }

   /* std::cout << "[Bone 생성] name=" << mName
        << ", posKeys=" << mPositions.size()
        << ", rotKeys=" << mRotations.size()
        << ", scaleKeys=" << mScales.size() << std::endl;*/
}

void Bone::Update(float animationTime)
{
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    mLocalTransform = translation * rotation * scale;


    //std::cout << "[Bone::Update] " << mName << " 시간=" << animationTime << "s" << std::endl;
}

glm::mat4 Bone::GetLocalTransform() const { return mLocalTransform; }
std::string Bone::GetBoneName() const { return mName; }
int Bone::GetBoneID() const { return mID; }

int Bone::GetPositionIndex(float time)
{
    for (int i = 0; i < mPositions.size() - 1; ++i)
        if (time < mPositions[i + 1].second)
            return i;
    return mPositions.size() - 2;
}

int Bone::GetRotationIndex(float time)
{
    for (int i = 0; i < mRotations.size() - 1; ++i)
        if (time < mRotations[i + 1].second)
            return i;
    return mRotations.size() - 2;
}

int Bone::GetScaleIndex(float time)
{
    for (int i = 0; i < mScales.size() - 1; ++i)
        if (time < mScales[i + 1].second)
            return i;
    return mScales.size() - 2;
}

float Bone::GetScaleFactor(float lastTime, float nextTime, float time)
{
    return (time - lastTime) / (nextTime - lastTime);
}

glm::mat4 Bone::InterpolatePosition(float time)
{
    if (mPositions.size() == 1)
        return glm::translate(glm::mat4(1.0f), mPositions[0].first);

    int i = GetPositionIndex(time);
    float factor = GetScaleFactor(mPositions[i].second, mPositions[i + 1].second, time);
    glm::vec3 pos = glm::mix(mPositions[i].first, mPositions[i + 1].first, factor);
    return glm::translate(glm::mat4(1.0f), pos);
}

glm::mat4 Bone::InterpolateRotation(float time)
{
    if (mRotations.size() == 1)
        return glm::toMat4(glm::normalize(mRotations[0].first));

    int i = GetRotationIndex(time);
    float factor = GetScaleFactor(mRotations[i].second, mRotations[i + 1].second, time);
    glm::quat rot = glm::slerp(mRotations[i].first, mRotations[i + 1].first, factor);
    return glm::toMat4(glm::normalize(rot));
}

glm::mat4 Bone::InterpolateScaling(float time)
{
    if (mScales.size() == 1)
        return glm::scale(glm::mat4(1.0f), mScales[0].first);

    int i = GetScaleIndex(time);
    float factor = GetScaleFactor(mScales[i].second, mScales[i + 1].second, time);
    glm::vec3 scale = glm::mix(mScales[i].first, mScales[i + 1].first, factor);
    return glm::scale(glm::mat4(1.0f), scale);
}