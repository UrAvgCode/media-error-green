﻿#include "collision_object.h"

#include <memory>
#include <string>
#include <vector>

#include "ofAppRunner.h"
#include "ofFbo.h"


CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", std::make_shared<EffectShader>()) {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::shared_ptr<EffectShader> effect_shader) :
    position(position), velocity(velocity), can_collide(false), effect_shader(effect_shader) {
    image.load(filename);
}

void CollisionObject::update(std::vector<Player> &players, const ofEasyCam &camera) {
    if (position.x <= 0 || position.x + width() >= ofGetWidth()) {
        velocity.x *= -1;
    }

    if (position.y <= 0 || position.y + height() >= ofGetHeight()) {
        velocity.y *= -1;
    }

    if (can_collide && check_collision_with_bodies(players, camera)) {
        velocity *= -1;
        can_collide = false;
    } else {
        can_collide = true;
    }

    position += velocity;
}

void CollisionObject::draw() const {
    effect_shader->begin();
    image.draw(position.x, position.y, 0);
    effect_shader->end();
}

bool CollisionObject::check_collision_with_bodies(std::vector<Player> &players, const ofEasyCam &camera) const {
    for (auto &player: players) {
        for (const auto &joint: player.get_skeleton().joints) {

            auto joint_position_homogeneous = glm::vec4(joint.position, 1.0f);
            auto rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            auto rotated_joint_position_homogenous = rotation_matrix * joint_position_homogeneous;
            auto rotated_joint_position = glm::vec3(rotated_joint_position_homogenous);

            auto joint_position = camera.worldToScreen(rotated_joint_position);
            if (joint_position.x > position.x && joint_position.x < position.x + width() &&
                joint_position.y > position.y && joint_position.y < position.y + height()) {

                player.set_shader(effect_shader);

                return true;
            }
        }
    }

    return false;
}

float CollisionObject::width() const { return image.getWidth(); }

float CollisionObject::height() const { return image.getHeight(); }
