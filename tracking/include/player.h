#pragma once

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofxAzureKinect.h>

class Player {
  public:
    Player();
    Player(std::uint32_t id, ofEasyCam *camera);

    void set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton);

    std::array<glm::vec2, K4ABT_JOINT_COUNT> get_projected_joints();

  private:
    std::uint32_t id;

    ofEasyCam *camera;

    ofxAzureKinect::BodySkeleton skeleton;

    ofShader effect_shader;
};
