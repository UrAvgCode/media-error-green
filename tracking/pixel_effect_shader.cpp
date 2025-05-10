#include "pixel_effect_shader.h"

PixelEffectShader::PixelEffectShader() {
    _shader.load("shaders/effect/pixel");
}

void PixelEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("block_size", 10.0f);
    _shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_player() { _shader.end(); }

void PixelEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("block_size", 6.0f);
    _shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_object() { _shader.end(); }
