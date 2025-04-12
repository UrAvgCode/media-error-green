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
    std::string get_id();
    std::vector<glm::vec3> get_skeleton_vertices() const { return skeleton_vertices; }
    void set_skeleton_vertices(const std::vector<glm::vec3> &vertices) { skeleton_vertices = vertices; }

    std::uint32_t id;

  private:
    ofEasyCam *camera;

    ofxAzureKinect::BodySkeleton skeleton;
    std::vector<glm::vec3> skeleton_vertices;

    ofShader effect_shader;
    std::string fake_shader;
};
