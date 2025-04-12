#include "player.h"

#include <ofAppRunner.h>
#include <ofGraphics.h>

#include "pixel_effect_shader.h"

Player::Player() : Player(0, nullptr) {}

Player::Player(int id, ofEasyCam *camera) : _id(id), camera(camera) {
    auto shader_settings = ofShaderSettings();
    shader_settings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render_player.vert";
    shader_settings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render_player.frag";
    shader_settings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
    shader_settings.bindDefaults = true;
    render_shader.setup(shader_settings);

    std::vector<glm::vec3> verts(1);
    player_vbo.setVertexData(verts.data(), static_cast<int>(verts.size()), GL_STATIC_DRAW);

    player_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    effect_shader = std::make_shared<EffectShader>();
}

void Player::render(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                    std::vector<int> &body_ids) {
    player_fbo.begin();
    ofClear(0, 0, 0, 0);

    camera->begin();
    {
        ofPushMatrix();
        {
            ofRotateXDeg(180);
            ofEnableDepthTest();

            render_shader.begin();
            {
                const auto frame_width = static_cast<int>(depth_tex.getWidth());
                const auto frame_height = static_cast<int>(depth_tex.getHeight());

                render_shader.setUniformTexture("depth_texture", depth_tex, 1);
                render_shader.setUniformTexture("body_index_texture", body_index_tex, 2);
                render_shader.setUniformTexture("world_texture", depth_to_world_tex, 3);

                render_shader.setUniform2i("frame_size", frame_width, frame_height);
                render_shader.setUniform1iv("body_ids", body_ids.data(), 6);

                render_shader.setUniform1i("player_id", _id);

                const int num_points = frame_width * frame_height;
                player_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
            }
            render_shader.end();

            ofDisableDepthTest();
        }
        ofPopMatrix();
    }
    camera->end();
    player_fbo.end();
}

void Player::draw() {
    effect_shader->begin();
    player_fbo.draw(0, 0);
    effect_shader->end();
}

void Player::set_skeleton(const ofxAzureKinect::BodySkeleton &skeleton) { this->skeleton = skeleton; }

void Player::set_fake_shader(std::string shader) {
    if (shader == "No. 0") {
        effect_shader = std::make_shared<PixelEffectShader>();
    } else {
        effect_shader = std::make_shared<EffectShader>();
    }

    fake_shader = shader;
}

int Player::id() const { return _id; }

std::string Player::get_fake_shader() const { return fake_shader; }

ofxAzureKinect::BodySkeleton Player::get_skeleton() const { return skeleton; }

std::array<glm::vec2, K4ABT_JOINT_COUNT> Player::get_projected_joints() const {
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
