#pragma once

#include <memory>
#include <string>
#include <vector>

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

    float width() const;
    float height() const;

  protected:
    void play_random_pluck();
    bool check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) const;

    glm::vec2 position;
    glm::vec2 velocity;

    ofFbo fbo;
    ofImage image;
    std::shared_ptr<EffectShader> effect_shader;

    bool can_collide;

    ofSoundPlayer pluck_b;
    ofSoundPlayer pluck_d;
    ofSoundPlayer pluck_e;
    ofSoundPlayer pluck_g;
    ofSoundPlayer global_effect;
};
