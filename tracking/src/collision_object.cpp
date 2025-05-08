#include "collision_object.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ofAppRunner.h>
#include <ofFbo.h>
#include <ofMath.h>

CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", std::make_shared<EffectShader>()) {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::shared_ptr<EffectShader> effect_shader) :
    _position(position), _velocity(velocity), _can_collide(false), _effect_shader(effect_shader) {
    _image.load(filename);
    _image.mirror(false, true);

    _pluck_b.load("resources/audio/gruen_pluck_b.wav");
    _pluck_d.load("resources/audio/gruen_pluck_d.wav");
    _pluck_e.load("resources/audio/gruen_pluck_e.wav");
    _pluck_g.load("resources/audio/gruen_pluck_g.wav");

    _pluck_b.setMultiPlay(true);
    _pluck_d.setMultiPlay(true);
    _pluck_e.setMultiPlay(true);
    _pluck_g.setMultiPlay(true);
}

void CollisionObject::update(std::vector<Player> &players, const ofEasyCam &camera) {
    if (_position.x <= 0 || _position.x + width() >= ofGetWidth()) {
        play_random_pluck();
        _velocity.x *= -1;
    }

    if (_position.y <= 0 || _position.y + height() >= ofGetHeight()) {
        play_random_pluck();
        _velocity.y *= -1;
    }

    if (auto [collided, dir] = check_collision_with_bodies(players, camera); collided) {
        if (_can_collide) {
            _velocity = dir;
            _can_collide = false;
        }
    } else {
        _can_collide = true;
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

    _position += _velocity;

    // clamp the position to the screen size
    _position.x = std::clamp(_position.x, 0.0f, static_cast<float>(ofGetWidth() - width()));
    _position.y = std::clamp(_position.y, 0.0f, static_cast<float>(ofGetHeight() - height()));
}

void CollisionObject::draw() const {
    _effect_shader->begin_object();
    _image.draw(_position.x, _position.y, 0);
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
    int random = static_cast<int>(ofRandom(0, 4));
    switch (random) {
        case 0:
            _pluck_b.play();
            break;
        case 1:
            _pluck_d.play();
            break;
        case 2:
            _pluck_e.play();
            break;
        case 3:
            _pluck_g.play();
            break;
        default:
            break;
    }
}

std::pair<bool, glm::vec2> CollisionObject::check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) {
    for (auto &player: players) {
        // Zugriff auf die Skeleton-Vertices des Spielers
        const auto &lines = player.get_skeleton_lines();
        const auto &velocities = player.get_skeleton_velocities();

        ofRectangle bounding_box(_position.x, _position.y, width(), height());
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto &line = lines[i];
            const auto &line_velocity = velocities[i];

            if (bounding_box.intersects(line[0], line[1])) {
                play_random_pluck();
                player.set_shader(_effect_shader);

                auto new_velocity = -_velocity;
                new_velocity += line_velocity[0];

                return {true, new_velocity};
            }
        }
    }

    return {false, {0, 0}};
}

float CollisionObject::width() const { return _image.getWidth(); }

float CollisionObject::height() const { return _image.getHeight(); }

glm::vec2 CollisionObject::position() const { return _position; }

std::shared_ptr<EffectShader> CollisionObject::effect_shader() const { return _effect_shader; }
