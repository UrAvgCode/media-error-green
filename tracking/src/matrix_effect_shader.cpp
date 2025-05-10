#include "matrix_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

MatrixEffectShader::MatrixEffectShader() {
    _shader.load("shaders/effect/matrix");
    _matrix.load("resources/matrix.png");
    _matrix.mirror(false, true);
}

void MatrixEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniformTexture("matrix_texture", _matrix, 1);
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
}

void MatrixEffectShader::end_player() { _shader.end(); }

void MatrixEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniformTexture("matrix_texture", _matrix, 1);
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
}

void MatrixEffectShader::end_object() { _shader.end(); }
