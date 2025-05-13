#include "player.h"

#include <memory>
#include <vector>

#include <ofAppRunner.h>
#include <ofGraphics.h>

#include "effect_shader.h"
#include "skeleton_utility.h"

Player::Player() : Player(0, ofxAzureKinect::BodySkeleton{}, nullptr) {}

Player::Player(int id, ofxAzureKinect::BodySkeleton skeleton, ofEasyCam *camera) :
    _id(id), _camera(camera), _skeleton(skeleton) {
    _player_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    _effect_shader = std::make_shared<EffectShader>();
}

void Player::render(const ofTexture &depth_tex, const ofTexture &body_index_tex, const ofTexture &depth_to_world_tex,
            const std::vector<int> &body_ids) {
    _player_fbo.begin();
    _effect_shader->draw_player(depth_tex, body_index_tex, depth_to_world_tex, body_ids, _camera, _id);
    _player_fbo.end();
}

void Player::draw() {
    _effect_shader->begin_player();
    _player_fbo.draw(0, 0);
    _effect_shader->end_player();
}

void Player::set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton) { this->_skeleton = skeleton; }

void Player::set_shader(std::shared_ptr<EffectShader> shader) { _effect_shader = shader; }

int Player::id() const { return _id; }

ofxAzureKinect::BodySkeleton Player::skeleton() const { return _skeleton; }

std::array<glm::vec2, K4ABT_JOINT_COUNT> Player::get_projected_joints() const {
    std::array<glm::vec2, K4ABT_JOINT_COUNT> projected_joints;

    for (std::size_t i = 0; i < K4ABT_JOINT_COUNT; ++i) {
        const auto joint = _skeleton.joints[i];

        auto joint_position_homogeneous = glm::vec4(joint.position, 1.0f);
        auto rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        auto rotated_joint_position_homogenous = rotation_matrix * joint_position_homogeneous;
        auto rotated_joint_position = glm::vec3(rotated_joint_position_homogenous);

        projected_joints[i] = _camera->worldToScreen(rotated_joint_position);
    }

    return projected_joints;
}

void Player::calculate_skeleton_lines() {
    _previous_skeleton_lines = _skeleton_lines;
    _skeleton_lines.clear();

    // Iteriere über die Verbindungen und füge die Vertices hinzu
    auto projected_joints = get_projected_joints();

    for (const auto &[start, end]: skeleton::connections) {
        const auto &start_joint = projected_joints[start];
        const auto &end_joint = projected_joints[end];

        // Prüfe, ob die Positionen gültig sind
        _skeleton_lines.push_back({start_joint, end_joint});
    }

    calculate_skeleton_velocities();
}

void Player::calculate_skeleton_velocities() {
    if (_previous_skeleton_lines.empty()) {
        _previous_skeleton_lines = _skeleton_lines;
    }

    _skeleton_velocities.clear();
    for (std::size_t i = 0; i < _skeleton_lines.size(); ++i) {
        const auto &current_line = _skeleton_lines[i];
        const auto &previous_line = _previous_skeleton_lines[i];

        glm::vec2 velocity_one = current_line[0] - previous_line[0];
        glm::vec2 velocity_two = current_line[1] - previous_line[1];
        _skeleton_velocities.push_back({velocity_one, velocity_two});
    }
}

std::vector<std::array<glm::vec2, 2>> Player::get_skeleton_lines() const { return _skeleton_lines; }

std::vector<std::array<glm::vec2, 2>> Player::get_skeleton_velocities() const { return _skeleton_velocities; }
