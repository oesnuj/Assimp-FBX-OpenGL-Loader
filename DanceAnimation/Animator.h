#pragma once
#include "Animation.h"
#include <vector>
#include <glm/glm.hpp>

class Animator {
public:
    Animator(Animation* animation);
    Animator(Animation* animation, const glm::mat4& globalInverse);


    void UpdateAnimation(float dt);
    void CalculateBoneTransform(const AssimpNodeData& node, const glm::mat4& parentTransform);
    std::vector<glm::mat4> GetFinalBoneMatrices() const;

private:
    int currentIndex = 0;
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;
    glm::mat4 globalInverseTransform;
};
