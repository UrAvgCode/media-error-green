#include "player.h"

#include <memory>
#include <vector>

#include <ofAppRunner.h>
#include <ofGraphics.h>

#include "pixel_effect_shader.h"
#include "skeleton_utility.h"

Player::Player() : Player(0, ofxAzureKinect::BodySkeleton{}, nullptr) {}

Player::Player(int id, ofxAzureKinect::BodySkeleton skeleton, ofEasyCam *camera) :
    _id(id), _camera(camera), _skeleton(skeleton) {
    auto shader_settings = ofShaderSettings();
    shader_settings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render_player.vert";
    shader_settings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render_player.frag";
    shader_settings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
    shader_settings.bindDefaults = true;
    _render_shader.setup(shader_settings);

    std::vector<glm::vec3> verts(1);
    _player_vbo.setVertexData(verts.data(), static_cast<int>(verts.size()), GL_STATIC_DRAW);

    _player_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    _effect_shader = std::make_shared<EffectShader>();
}

void Player::render(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                    std::vector<int> &body_ids) {
    _player_fbo.begin();
    ofClear(0, 0, 0, 0);

    _camera->begin();
    {
        ofPushMatrix();
        {
            ofRotateXDeg(180);
            ofEnableDepthTest();

            _render_shader.begin();
            {
                const auto frame_width = static_cast<int>(depth_tex.getWidth());
                const auto frame_height = static_cast<int>(depth_tex.getHeight());

                _render_shader.setUniformTexture("depth_texture", depth_tex, 1);
                _render_shader.setUniformTexture("body_index_texture", body_index_tex, 2);
                _render_shader.setUniformTexture("world_texture", depth_to_world_tex, 3);

                _render_shader.setUniform2i("frame_size", frame_width, frame_height);
                _render_shader.setUniform1iv("body_ids", body_ids.data(), 6);

                _render_shader.setUniform1i("player_id", _id);

                const int num_points = frame_width * frame_height;
                _player_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
            }
            _render_shader.end();

            ofDisableDepthTest();
        }
        ofPopMatrix();
    }
    _camera->end();
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
