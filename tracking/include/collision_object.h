#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ofEasyCam.h>
#include <ofFbo.h>
#include <ofImage.h>
#include <ofSoundPlayer.h>
#include <ofxAzureKinect.h>

#include "effect_shader.h"
#include "player.h"

class CollisionObject {
  public:
    CollisionObject();
    CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                    std::shared_ptr<EffectShader> effect_shader);

    void update(std::vector<Player> &players, const std::vector<CollisionObject> &objects, const ofEasyCam &camera);
    void draw() const;

    std::pair<bool, glm::vec2> global_effect_triggered();

    void set_effect_shader(std::shared_ptr<EffectShader>);

    float width() const;
    float height() const;
    glm::vec2 position() const;
    glm::vec2 velocity() const;
    std::shared_ptr<EffectShader> effect_shader() const;

  protected:
    void play_random_pluck();
    std::pair<bool, glm::vec2> check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera);
    std::pair<bool, glm::vec2> check_collision_with_objects(const std::vector<CollisionObject> &objects);

    const float _min_speed = 5;
    const float _max_speed = 20;
    const float _friction = 0.99f;

    glm::vec2 _position;
    glm::vec2 _velocity;

    const float _fbo_padding;

    ofFbo _fbo;
    ofImage _image;
    std::shared_ptr<EffectShader> _effect_shader;

    bool _can_collide;

    std::vector<ofSoundPlayer> puck_sounds;
};
