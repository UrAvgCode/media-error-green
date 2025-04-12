#pragma once

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofxAzureKinect.h>

class Player {
  public:
    Player();
    Player(std::uint32_t id, ofEasyCam *camera);

    void set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton);
    ofxAzureKinect::BodySkeleton get_skeleton() const;
    void set_fake_shader(std::string shader);
    std::string get_fake_shader();
    std::array<glm::vec2, K4ABT_JOINT_COUNT> get_projected_joints();
    void calculate_skeleton_vertices();
    std::string get_id();
    std::vector<std::array<glm::vec2, 2>> get_skeleton_vertices() const { return skeleton_vertices; }

    std::uint32_t id;

  private:
    ofEasyCam *camera;

    ofxAzureKinect::BodySkeleton skeleton;
    std::vector<std::array<glm::vec2, 2>> skeleton_vertices;

    ofShader effect_shader;
    std::string fake_shader;
};
