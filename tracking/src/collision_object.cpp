#include "collision_object.h"

#include <string>
#include <vector>

#include "ofAppRunner.h"


CollisionObject::CollisionObject() : CollisionObject({0, 0}, {0, 0}, "", "") {}

CollisionObject::CollisionObject(glm::vec2 position, glm::vec2 velocity, const std::string &filename,
                                 std::string logo_shader) :
    position(position), velocity(velocity), can_collide(false), logo_shader(logo_shader) {
    image.load(filename);

    pluck_b.load("resources/audio/gruen_pluck_b.wav");
    pluck_d.load("resources/audio/gruen_pluck_d.wav");
    pluck_e.load("resources/audio/gruen_pluck_e.wav");
    pluck_g.load("resources/audio/gruen_pluck_g.wav");
    globalEffect.load("resources/audio/gruen_globalEffect.wav");

    pluck_b.setMultiPlay(true);
    pluck_d.setMultiPlay(true);
    pluck_e.setMultiPlay(true);
    pluck_g.setMultiPlay(true);
    globalEffect.setMultiPlay(true);
}

std::string CollisionObject::get_fake_shader() { return logo_shader; }

void CollisionObject::draw() const { image.draw(position.x, position.y); }

float CollisionObject::width() const { return image.getWidth(); }

float CollisionObject::height() const { return image.getHeight(); }

void CollisionObject::update(std::vector<Player> &players, const ofEasyCam &camera) {
    ofSoundUpdate();

    if (position.x <= 0 || position.x + width() >= ofGetWidth()) {
        velocity.x *= -1;
        playRandomPluck();
    }

    if (position.y <= 0 || position.y + height() >= ofGetHeight()) {
        velocity.y *= -1;
        playRandomPluck();
    }

    if (can_collide && check_collision_with_bodies(players, camera)) {
        velocity *= -1;
        can_collide = false;
    } else {
        can_collide = true;
    }

    //When obj hits top left corner
    if (position.x <= 5 && position.y <= 5) {
        globalEffect.setVolume(0.1f);
        globalEffect.play();
    }

    position += velocity;
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
                affect_player(player, logo_shader);
                return true;
            }
        }
    }

    return false;
}

void CollisionObject::affect_player(Player &player, std::string shader) const {
    if (shader.compare(player.get_fake_shader()) == 0) {
        player.set_fake_shader("none");
    } else {
        player.set_fake_shader(shader);
    }
}

void CollisionObject::playRandomPluck() {
    int random = ofRandom(0,4);
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
