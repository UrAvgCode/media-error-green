#pragma once

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofxAzureKinect.h>

class Player {
  public:
    Player();
    Player(std::uint32_t id, ofEasyCam *camera);

    void set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton);
    void set_fake_shader(std::string shader);
    std::string get_fake_shader();
    ofxAzureKinect::BodySkeleton get_skeleton();

    std::array<glm::vec2, K4ABT_JOINT_COUNT> get_projected_joints();
    std::string get_id();

    std::uint32_t id;

  private:
    ofEasyCam *camera;

    ofxAzureKinect::BodySkeleton skeleton;

    ofShader effect_shader;
    std::string fake_shader;
};
