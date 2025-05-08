#include "pixel_effect_shader.h"

PixelEffectShader::PixelEffectShader() { 
    _player_shader.load("shaders/player_effects/pixel");
    _object_shader.load("shaders/object_effects/pixel");
}

void PixelEffectShader::begin_player() {
    _player_shader.begin();
    _player_shader.setUniform1f("block_size", 10.0f);
    _player_shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_player() { _player_shader.end(); }

void PixelEffectShader::begin_object() {
    _object_shader.begin();
    _object_shader.setUniform1f("block_size", 10.0f);
    _object_shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_object() { _object_shader.end(); }
