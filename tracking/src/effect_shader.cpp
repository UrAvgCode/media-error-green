#include "effect_shader.h"

#include <string>
#include <vector>

#include <k4abttypes.h>

#include <ofGraphics.h>

EffectShader::EffectShader() : EffectShader("shaders/render/default") {}

EffectShader::EffectShader(const std::string &shader) : EffectShader(shader + ".vert", shader + ".frag") {}

EffectShader::EffectShader(const std::string &vertex_shader, const std::string &fragment_shader) {
    auto verts = std::vector<glm::vec3>(1);
    _player_vbo.setVertexData(verts.data(), static_cast<int>(verts.size()), GL_STATIC_DRAW);

    auto shader_settings = ofShaderSettings();
    shader_settings.shaderFiles[GL_VERTEX_SHADER] = vertex_shader;
    shader_settings.shaderFiles[GL_FRAGMENT_SHADER] = fragment_shader;
    shader_settings.intDefines["BODY_INDEX_MAP_BACKGROUND"] = K4ABT_BODY_INDEX_MAP_BACKGROUND;
    shader_settings.bindDefaults = true;
    _render_shader.setup(shader_settings);
}

void EffectShader::begin_player() {}

void EffectShader::end_player() {}

void EffectShader::begin_object() {}

void EffectShader::end_object() {}

void EffectShader::draw_player(const ofTexture &depth_tex, const ofTexture &body_index_tex,
                               const ofTexture &depth_to_world_tex, const std::vector<int> &body_ids, ofEasyCam *camera,
                               int id) {
    ofClear(0, 0, 0, 0);

    camera->begin();
    ofPushMatrix();
    ofRotateXDeg(180);
    ofEnableDepthTest();

    _render_shader.begin();
    {
        const auto frame_width = static_cast<int>(depth_tex.getWidth());
        const auto frame_height = static_cast<int>(depth_tex.getHeight());
        const auto elapsed_time = static_cast<int>(ofGetElapsedTimeMillis());

        _render_shader.setUniformTexture("depth_texture", depth_tex, 1);
        _render_shader.setUniformTexture("body_index_texture", body_index_tex, 2);
        _render_shader.setUniformTexture("world_texture", depth_to_world_tex, 3);

        _render_shader.setUniform2i("frame_size", frame_width, frame_height);
        _render_shader.setUniform1iv("body_ids", body_ids.data(), 6);

        _render_shader.setUniform1i("player_id", id);

        _render_shader.setUniform1i("time", elapsed_time);

        const int num_points = frame_width * frame_height;
        _player_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
    }
    _render_shader.end();

    ofDisableDepthTest();
    ofPopMatrix();
    camera->end();
}
