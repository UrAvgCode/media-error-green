#include "matrix_effect_shader.h"

#include <vector>

#include <ofAppRunner.h>
#include <ofUtils.h>

MatrixEffectShader::MatrixEffectShader() : EffectShader("shaders/render/matrix") {
    _shader.load("shaders/effect/matrix");

    _matrix.load("resources/matrix.png");
    _matrix.mirror(false, true);

    _matrix_logo.load("resources/matrix_logo.png");
    _matrix_logo.mirror(false, true);
}

void MatrixEffectShader::begin_player() {}

void MatrixEffectShader::end_player() {}

void MatrixEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniformTexture("matrix_texture", _matrix_logo, 1);
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
    _shader.setUniform1f("speed", 0.05);
}

void MatrixEffectShader::end_object() { _shader.end(); }

void MatrixEffectShader::draw_player(const ofTexture &depth_tex, const ofTexture &body_index_tex,
                                     const ofTexture &depth_to_world_tex, const std::vector<int> &body_ids,
                                     ofEasyCam *camera, int id) {
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

        _render_shader.setUniformTexture("matrix_texture", _matrix, 4);

        const int num_points = frame_width * frame_height;
        _player_vbo.drawInstanced(GL_POINTS, 0, 1, num_points);
    }
    _render_shader.end();

    ofDisableDepthTest();
    ofPopMatrix();
    camera->end();
}
