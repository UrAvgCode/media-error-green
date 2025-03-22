#include "collision_object.h"

#include <string>
#include <vector>

#include "ofAppRunner.h"


CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "") {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename) :
    position(position), velocity(velocity), can_collide(false) {
    image.load(filename);
}

std::string CollisionObject::get_fake_shader() { return fake_shader; }

void CollisionObject::draw() const { image.draw(position.x, position.y); }

float CollisionObject::width() const { return image.getWidth(); }

float CollisionObject::height() const { return image.getHeight(); }

void CollisionObject::update(std::map<std::uint32_t, Player> &players, const ofEasyCam &camera) {
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

bool CollisionObject::check_collision_with_bodies( std::map<std::uint32_t, Player> &players,
                                                  const ofEasyCam &camera) const {
    for (auto &player_pair: players) {
        Player &player = player_pair.second;
        for (const auto &joint: player.get_skeleton().joints) {

            auto joint_position_homogeneous = glm::vec4(joint.position, 1.0f);
            auto rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            auto rotated_joint_position_homogenous = rotation_matrix * joint_position_homogeneous;
            auto rotated_joint_position = glm::vec3(rotated_joint_position_homogenous);

            auto joint_position = camera.worldToScreen(rotated_joint_position);
            if (joint_position.x > position.x && joint_position.x < position.x + width() &&
                joint_position.y > position.y && joint_position.y < position.y + height()) {
                affect_player(player, fake_shader);
                return true;
            }
        }
    }

    return false;
}

void CollisionObject::affect_player(Player &player, std::string shader) const { 
    if (shader.compare(fake_shader)) {
        player.set_fake_shader("none"); 
    }
    else {
        player.set_fake_shader(shader);
    }        
}
