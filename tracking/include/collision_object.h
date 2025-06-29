#pragma once

#include <memory>
#include <optional>
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

    void update(std::vector<Player> &players, const std::vector<CollisionObject> &objects);
    void update_position();
    void draw() const;

    std::pair<bool, glm::vec2> global_effect_triggered();

    void set_effect_shader(std::shared_ptr<EffectShader>);

    float width() const;
    float height() const;
    glm::vec2 position() const;
    glm::vec2 velocity() const;
    std::shared_ptr<EffectShader> effect_shader() const;

  private:
    void play_random_pluck();
    void play_random_noise_hit();

    std::optional<glm::vec2> check_collision_with_bodies(std::vector<Player> &players) const;
    std::optional<glm::vec2> check_collision_with_objects(const std::vector<CollisionObject> &objects) const;

    const float _min_speed = 5;
    const float _max_speed = 20;
    const float _friction = 0.99f;

    glm::vec2 _position;
    glm::vec2 _velocity;

    const float _fbo_padding;

    ofFbo _fbo;
    ofImage _image;
    std::shared_ptr<EffectShader> _effect_shader;

    std::vector<ofSoundPlayer> _pluck_sounds;
    std::vector<ofSoundPlayer> _noise_sounds;

    bool _can_collide_with_player;
    bool _can_collide_with_object;
};
