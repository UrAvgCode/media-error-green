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
    _position(position), _velocity(velocity), can_collide(false), effect_shader(effect_shader) {
    image.load(filename);

    pluck_b.load("resources/audio/gruen_pluck_b.wav");
    pluck_d.load("resources/audio/gruen_pluck_d.wav");
    pluck_e.load("resources/audio/gruen_pluck_e.wav");
    pluck_g.load("resources/audio/gruen_pluck_g.wav");
    global_effect.load("resources/audio/gruen_globalEffect.wav");

    pluck_b.setMultiPlay(true);
    pluck_d.setMultiPlay(true);
    pluck_e.setMultiPlay(true);
    pluck_g.setMultiPlay(true);
    global_effect.setMultiPlay(false);
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
        if (can_collide) {
            _velocity = dir;
            can_collide = false;
        }
    } else {
        can_collide = true;
    }

    _velocity *= friction;

    auto speed = glm::length(_velocity);
    if (speed != 0) {
        if (speed < min_speed) {
            _velocity *= min_speed / speed;
        } else if (speed > max_speed) {
            _velocity *= max_speed / speed;
        }
    }

    _position += _velocity;

    // clamp the position to the screen size
    _position.x = std::clamp(_position.x, 0.0f, static_cast<float>(ofGetWidth() - width()));
    _position.y = std::clamp(_position.y, 0.0f, static_cast<float>(ofGetHeight() - height()));
}

void CollisionObject::draw() const {
    effect_shader->begin_object();
    image.draw(_position.x, _position.y, 0);
    effect_shader->end_object();
}

bool CollisionObject::global_effect_triggered() {
    auto bounding_box = ofRectangle(_position.x, _position.y, width(), height());

    float corner_size = 50.0f;
    auto top_left = ofRectangle(0.0f, 0.0f, corner_size, corner_size);
    auto top_right = ofRectangle(ofGetWidth() - corner_size, 0.0f, corner_size, corner_size);
    auto bottom_left = ofRectangle(0.0f, ofGetHeight() - corner_size, corner_size, corner_size);
    auto bottom_right = ofRectangle(ofGetWidth() - corner_size, ofGetHeight() - corner_size, corner_size, corner_size);

    auto corners = std::array<ofRectangle, 4>({top_left, top_right, bottom_left, bottom_right});

    for (const auto &corner: corners) {
        if (bounding_box.intersects(corner)) {
            // global_effect.setVolume(0.06f);
            // global_effect.play();

            return true;
        }
    }

    return false;
}

void CollisionObject::play_random_pluck() {
    int random = static_cast<int>(ofRandom(0, 4));
    switch (random) {
        case 0:
            pluck_b.play();
            break;
        case 1:
            pluck_d.play();
            break;
        case 2:
            pluck_e.play();
            break;
        case 3:
            pluck_g.play();
            break;
        default:
            break;
    }
}

std::pair<bool, glm::vec2> CollisionObject::check_collision_with_bodies(std::vector<Player> &players,
                                                                        const ofEasyCam &camera) const {
    for (auto &player: players) {
        // Zugriff auf die Skeleton-Vertices des Spielers
        const auto &lines = player.get_skeleton_lines();
        const auto &velocities = player.get_skeleton_velocities();

        ofRectangle bounding_box(_position.x, _position.y, width(), height());
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto &line = lines[i];
            const auto &line_velocity = velocities[i];

            if (bounding_box.intersects(line[0], line[1])) {
                player.set_shader(effect_shader);

                auto new_velocity = -_velocity;
                new_velocity += line_velocity[0];

                return {true, new_velocity};
            }
        }
    }

    return {false, {0, 0}};
}

float CollisionObject::width() const { return image.getWidth(); }

float CollisionObject::height() const { return image.getHeight(); }

glm::vec2 CollisionObject::position() const { return _position; }
