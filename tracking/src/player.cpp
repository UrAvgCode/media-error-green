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

void Player::calculate_skeleton_vertices() {
    skeleton_vertices.clear();
    // Definiere die Verbindungen zwischen den Joints
    const std::vector<std::pair<int, int>> connections = {
            {K4ABT_JOINT_PELVIS, K4ABT_JOINT_SPINE_NAVEL},
            {K4ABT_JOINT_SPINE_NAVEL, K4ABT_JOINT_SPINE_CHEST},
            {K4ABT_JOINT_SPINE_CHEST, K4ABT_JOINT_NECK},
            {K4ABT_JOINT_NECK, K4ABT_JOINT_HEAD},

            // Kopf
            {K4ABT_JOINT_HEAD, K4ABT_JOINT_NOSE},
            {K4ABT_JOINT_NOSE, K4ABT_JOINT_EYE_LEFT},
            {K4ABT_JOINT_EYE_LEFT, K4ABT_JOINT_EAR_LEFT},
            {K4ABT_JOINT_NOSE, K4ABT_JOINT_EYE_RIGHT},
            {K4ABT_JOINT_EYE_RIGHT, K4ABT_JOINT_EAR_RIGHT},

            // Linkes Bein
            {K4ABT_JOINT_PELVIS, K4ABT_JOINT_HIP_LEFT},
            {K4ABT_JOINT_HIP_LEFT, K4ABT_JOINT_KNEE_LEFT},
            {K4ABT_JOINT_KNEE_LEFT, K4ABT_JOINT_ANKLE_LEFT},
            {K4ABT_JOINT_ANKLE_LEFT, K4ABT_JOINT_FOOT_LEFT},

            // Rechtes Bein
            {K4ABT_JOINT_PELVIS, K4ABT_JOINT_HIP_RIGHT},
            {K4ABT_JOINT_HIP_RIGHT, K4ABT_JOINT_KNEE_RIGHT},
            {K4ABT_JOINT_KNEE_RIGHT, K4ABT_JOINT_ANKLE_RIGHT},
            {K4ABT_JOINT_ANKLE_RIGHT, K4ABT_JOINT_FOOT_RIGHT},

            // Linker Arm
            {K4ABT_JOINT_NECK, K4ABT_JOINT_CLAVICLE_LEFT},
            {K4ABT_JOINT_CLAVICLE_LEFT, K4ABT_JOINT_SHOULDER_LEFT},
            {K4ABT_JOINT_SHOULDER_LEFT, K4ABT_JOINT_ELBOW_LEFT},
            {K4ABT_JOINT_ELBOW_LEFT, K4ABT_JOINT_WRIST_LEFT},

            // Rechter Arm
            {K4ABT_JOINT_NECK, K4ABT_JOINT_CLAVICLE_RIGHT},
            {K4ABT_JOINT_CLAVICLE_RIGHT, K4ABT_JOINT_SHOULDER_RIGHT},
            {K4ABT_JOINT_SHOULDER_RIGHT, K4ABT_JOINT_ELBOW_RIGHT},
            {K4ABT_JOINT_ELBOW_RIGHT, K4ABT_JOINT_WRIST_RIGHT},
    };

    // Iteriere über die Verbindungen und füge die Vertices hinzu
    auto projected_joints = get_projected_joints();

    for (const auto &[start, end]: connections) {
        const auto &start_joint = projected_joints[start];
        const auto &end_joint = projected_joints[end];

        // Prüfe, ob die Positionen gültig sind
        skeleton_vertices.push_back({start_joint, end_joint});
    }
}
