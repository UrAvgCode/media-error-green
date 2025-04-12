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
    Player(int id, ofEasyCam *camera);

    void render(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                std::vector<int> &body_ids);

    void draw();

    void set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton);
    void set_shader(std::shared_ptr<EffectShader> shader);

    int id() const;
    ofxAzureKinect::BodySkeleton get_skeleton() const;
    std::array<glm::vec2, K4ABT_JOINT_COUNT> get_projected_joints() const;

  private:
    int _id;

    ofEasyCam *camera;

    ofxAzureKinect::BodySkeleton skeleton;

    ofShader render_shader;
    ofVbo player_vbo;
    ofFbo player_fbo;

    std::shared_ptr<EffectShader> effect_shader;
};
