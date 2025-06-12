#pragma once
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <glm/gtc/quaternion.hpp>


class Bone {
public:
    Bone() = default;
    Bone(const std::string& name, int id, const aiNodeAnim* channel);

    void Update(float animationTime);
    glm::mat4 GetLocalTransform() const;
    std::string GetBoneName() const;
    int GetBoneID() const;

private:
    std::string mName;
    int mID;

    // Bone.h
    std::vector<std::pair<glm::vec3, float>> mPositions;
    std::vector<std::pair<glm::quat, float>> mRotations;
    std::vector<std::pair<glm::vec3, float>> mScales;


    glm::mat4 mLocalTransform;

    int GetPositionIndex(float time);
    int GetRotationIndex(float time);
    int GetScaleIndex(float time);

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float currentTime);

    glm::mat4 InterpolatePosition(float time);
    glm::mat4 InterpolateRotation(float time);
    glm::mat4 InterpolateScaling(float time);
};
