#include "matrix_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

MatrixEffectShader::MatrixEffectShader() {
    _shader.load("shaders/effect/matrix");

    _matrix.load("resources/matrix.png");
    _matrix.mirror(false, true);

    _matrix_logo.load("resources/matrix_logo.png");
    _matrix_logo.mirror(false, true);
}

void MatrixEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniformTexture("matrix_texture", _matrix, 1);
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
    _shader.setUniform1f("speed", 0.1);
}

void MatrixEffectShader::end_player() { _shader.end(); }

void MatrixEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniformTexture("matrix_texture", _matrix_logo, 1);
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
    _shader.setUniform1f("speed", 0.05);
}

void MatrixEffectShader::end_object() { _shader.end(); }
