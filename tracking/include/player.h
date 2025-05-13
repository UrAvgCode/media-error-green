#pragma once

#include <memory>
#include <vector>

#include <ofEasyCam.h>
#include <ofFbo.h>
#include <ofShader.h>
#include <ofxAzureKinect.h>

#include "effect_shader.h"

class Player {
  public:
    Player();
    Player(int id, ofxAzureKinect::BodySkeleton skeleton, ofEasyCam *camera);

    void render(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                std::vector<int> &body_ids);

    void draw();

    void calculate_skeleton_lines();

    void set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton);
    void set_shader(std::shared_ptr<EffectShader> shader);

    int id() const;
    ofxAzureKinect::BodySkeleton skeleton() const;
    std::array<glm::vec2, K4ABT_JOINT_COUNT> get_projected_joints() const;
    std::vector<std::array<glm::vec2, 2>> get_skeleton_lines() const;
    std::vector<std::array<glm::vec2, 2>> get_skeleton_velocities() const;

  private:
    void calculate_skeleton_velocities();

    int _id;

    ofEasyCam *_camera;

    ofxAzureKinect::BodySkeleton _skeleton;

    ofFbo _player_fbo;

    std::shared_ptr<EffectShader> _effect_shader;

    std::vector<std::array<glm::vec2, 2>> _skeleton_lines;
    std::vector<std::array<glm::vec2, 2>> _previous_skeleton_lines;
    std::vector<std::array<glm::vec2, 2>> _skeleton_velocities;
};
