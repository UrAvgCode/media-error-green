#pragma once

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofxAzureKinect.h>

class Player {
  public:
    Player();
    Player(std::uint32_t id, ofEasyCam *camera);

    void draw(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                  std::vector<int> &body_ids);

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

    ofShader render_shader;
    ofVbo player_vbo;
};
