#include "player.h"
#include <ofGraphics.h>

Player::Player() : Player(0, nullptr) {}

Player::Player(std::uint32_t id, ofEasyCam *camera) : id(id), camera(camera) {}

void Player::set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton) { this->skeleton = skeleton; }

void Player::set_fake_shader(std::string shader) { fake_shader = shader; }
std::string Player::get_fake_shader() { return fake_shader; }
std::string Player::get_id() { return std::to_string(id); }

ofxAzureKinect::BodySkeleton Player::get_skeleton() const { return skeleton; }


std::array<glm::vec2, K4ABT_JOINT_COUNT> Player::get_projected_joints() {
    std::array<glm::vec2, K4ABT_JOINT_COUNT> projected_joints;

    for (std::size_t i = 0; i < K4ABT_JOINT_COUNT; ++i) {
        const auto joint = skeleton.joints[i];

        auto joint_position_homogeneous = glm::vec4(joint.position, 1.0f);
        auto rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        auto rotated_joint_position_homogenous = rotation_matrix * joint_position_homogeneous;
        auto rotated_joint_position = glm::vec3(rotated_joint_position_homogenous);

        projected_joints[i] = camera->worldToScreen(rotated_joint_position);
    }

    return projected_joints;
}
