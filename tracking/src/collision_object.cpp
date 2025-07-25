﻿#include "collision_object.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ofAppRunner.h>
#include <ofFbo.h>
#include <ofGraphics.h>
#include <ofMath.h>

CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", std::make_shared<EffectShader>()) {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::shared_ptr<EffectShader> effect_shader) :
    _position(position), _velocity(velocity), _can_collide_with_player(false), _can_collide_with_object(false),
    _effect_shader(effect_shader), _fbo_padding(50) {
    _image.load(filename);
    _image.mirror(false, true);

    _fbo.allocate(_image.getWidth() + 2 * _fbo_padding, _image.getHeight() + 2 * _fbo_padding, GL_RGBA);

    _fbo.begin();
    _image.draw(_fbo_padding, _fbo_padding);
    _fbo.end();

    auto pluck_filenames = std::vector<std::string>({"pluck_1.wav", "pluck_2.wav", "pluck_3.wav", "pluck_4.wav"});
    auto noise_filenames = std::vector<std::string>({"noise_hit_1.wav", "noise_hit_2.wav", "noise_hit_3.wav"});

    for (const auto &filename: pluck_filenames) {
        auto sound_player = ofSoundPlayer();

        sound_player.load("resources/audio/" + filename);
        sound_player.setVolume(0.075f);
        sound_player.setMultiPlay(true);

        _pluck_sounds.push_back(sound_player);
    }

    for (const auto &filename: noise_filenames) {
        auto sound_player = ofSoundPlayer();

        sound_player.load("resources/audio/" + filename);
        sound_player.setVolume(0.15f);
        sound_player.setMultiPlay(true);

        _noise_sounds.push_back(sound_player);
    }
}

void CollisionObject::update(std::vector<Player> &players, const std::vector<CollisionObject> &objects) {
    if (_position.x <= 0 || _position.x + width() >= ofGetWidth()) {
        play_random_pluck();
        _velocity.x *= -1;
    }

    if (_position.y <= 0 || _position.y + height() >= ofGetHeight()) {
        play_random_pluck();
        _velocity.y *= -1;
    }

    if (auto direction = check_collision_with_objects(objects); direction.has_value()) {
        if (_can_collide_with_object) {
            play_random_pluck();
            _velocity = direction.value();
            _can_collide_with_object = false;
        }
    } else {
        _can_collide_with_object = true;
    }

    if (auto direction = check_collision_with_bodies(players); direction.has_value()) {
        if (_can_collide_with_player) {
            play_random_noise_hit();
            _velocity = direction.value();
            _can_collide_with_player = false;
        }
    } else {
        _can_collide_with_player = true;
    }

    _velocity *= _friction;

    auto speed = glm::length(_velocity);
    if (speed != 0) {
        if (speed < _min_speed) {
            _velocity *= _min_speed / speed;
        } else if (speed > _max_speed) {
            _velocity *= _max_speed / speed;
        }
    }
}

void CollisionObject::update_position() {
    _position += _velocity;

    // clamp the position to the screen size
    _position.x = std::clamp(_position.x, 0.0f, static_cast<float>(ofGetWidth() - width()));
    _position.y = std::clamp(_position.y, 0.0f, static_cast<float>(ofGetHeight() - height()));
}

void CollisionObject::draw() const {
    _effect_shader->begin_object();
    _fbo.draw(_position.x - _fbo_padding, _position.y - _fbo_padding);
    _effect_shader->end_object();
}

std::pair<bool, glm::vec2> CollisionObject::global_effect_triggered() {
    auto bounding_box = ofRectangle(_position.x, _position.y, width(), height());

    float corner_size = 50.0f;
    auto top_left = ofRectangle(0.0f, 0.0f, corner_size, corner_size);
    auto top_right = ofRectangle(ofGetWidth() - corner_size, 0.0f, corner_size, corner_size);
    auto bottom_left = ofRectangle(0.0f, ofGetHeight() - corner_size, corner_size, corner_size);
    auto bottom_right = ofRectangle(ofGetWidth() - corner_size, ofGetHeight() - corner_size, corner_size, corner_size);

    auto corners = std::array<ofRectangle, 4>({top_left, top_right, bottom_left, bottom_right});

    auto corner_positions =
            std::array<glm::vec2, 4>({{0, 0}, {ofGetWidth(), 0}, {0, ofGetHeight()}, {ofGetWidth(), ofGetHeight()}});

    for (std::size_t i = 0; i < corners.size(); ++i) {
        const auto &corner = corners[i];
        const auto &corner_position = corner_positions[i];

        if (bounding_box.intersects(corner)) {
            return {true, corner_position};
        }
    }

    return {false, {0, 0}};
}

void CollisionObject::set_effect_shader(std::shared_ptr<EffectShader> shader) { _effect_shader = shader; }

void CollisionObject::play_random_pluck() {
    auto random = static_cast<std::size_t>(ofRandom(_pluck_sounds.size()));
    _pluck_sounds[random].play();
}

void CollisionObject::play_random_noise_hit() {
    auto random = static_cast<std::size_t>(ofRandom(_noise_sounds.size()));
    _noise_sounds[random].play();
}

std::optional<glm::vec2> CollisionObject::check_collision_with_bodies(std::vector<Player> &players) const {
    for (auto &player: players) {
        const auto &lines = player.get_skeleton_lines();
        const auto &velocities = player.get_skeleton_velocities();

        ofRectangle bounding_box(_position.x, _position.y, width(), height());
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto &line = lines[i];
            const auto &line_velocity = velocities[i];

            if (bounding_box.intersects(line[0], line[1])) {
                player.set_shader(_effect_shader);

                auto new_velocity = -(_velocity);
                new_velocity += line_velocity[0];

                return new_velocity;
            }
        }
    }

    return std::nullopt;
}

std::optional<glm::vec2>
CollisionObject::check_collision_with_objects(const std::vector<CollisionObject> &objects) const {
    auto this_bounds = ofRectangle(position().x, position().y, width(), height());
    for (const auto &object: objects) {
        if (this == &object) {
            continue;
        }

        auto other_bounds = ofRectangle(object.position().x, object.position().y, object.width(), object.height());
        if (this_bounds.intersects(other_bounds)) {
            glm::vec2 new_velocity = _velocity;

            // Calculate overlap on X and Y axes
            float overlap_left = this_bounds.getRight() - other_bounds.getLeft();
            float overlap_right = other_bounds.getRight() - this_bounds.getLeft();
            float overlap_top = this_bounds.getBottom() - other_bounds.getTop();
            float overlap_bottom = other_bounds.getBottom() - this_bounds.getTop();

            float x_overlap = std::min(overlap_left, overlap_right);
            float y_overlap = std::min(overlap_top, overlap_bottom);

            const auto intersection = this_bounds.getIntersection(other_bounds);

            if (x_overlap < y_overlap) {
                new_velocity.x = -(new_velocity.x); // bounce horizontally
            } else {
                new_velocity.y = -(new_velocity.y); // bounce vertically
            }

            return new_velocity;
        }
    }

    return std::nullopt;
}

float CollisionObject::width() const { return _image.getWidth(); }

float CollisionObject::height() const { return _image.getHeight(); }

glm::vec2 CollisionObject::position() const { return _position; }

glm::vec2 CollisionObject::velocity() const { return _velocity; }

std::shared_ptr<EffectShader> CollisionObject::effect_shader() const { return _effect_shader; }
