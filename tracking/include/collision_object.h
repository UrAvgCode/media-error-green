#pragma once

#include <memory>
#include <string>
#include <vector>
#include <utility>

#include <ofEasyCam.h>
#include <ofFbo.h>
#include <ofImage.h>
#include <ofxAzureKinect.h>
#include <ofSoundPlayer.h>

#include "effect_shader.h"
#include "player.h"

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                    std::shared_ptr<EffectShader> effect_shader);

    void update(std::vector<Player> &players, const ofEasyCam &camera);
    void draw() const;

    std::pair<bool, glm::vec2> global_effect_triggered();

    void set_effect_shader(std::shared_ptr<EffectShader>);

    float width() const;
    float height() const;
    glm::vec2 position() const;
    std::shared_ptr<EffectShader> effect_shader() const;

  protected:
    const float min_speed = 5;
    const float max_speed = 20;
    const float friction = 0.99f;

    glm::vec2 _position;
    glm::vec2 _velocity;

    ofFbo fbo;
    ofImage image;
    std::shared_ptr<EffectShader> _effect_shader;

    bool can_collide;

    ofSoundPlayer pluck_b;
    ofSoundPlayer pluck_d;
    ofSoundPlayer pluck_e;
    ofSoundPlayer pluck_g;
    ofSoundPlayer global_effect;

    void play_random_pluck();
    std::pair<bool, glm::vec2> check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) const;
};
