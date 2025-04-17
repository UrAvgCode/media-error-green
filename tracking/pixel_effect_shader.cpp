#include "pixel_effect_shader.h"

PixelEffectShader::PixelEffectShader() { 
    player_shader.load("shaders/player_effects/pixel");
    object_shader.load("shaders/object_effects/pixel");
}

void PixelEffectShader::begin_player() {
    player_shader.begin();
    player_shader.setUniform1f("block_size", 10.0f);
    player_shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_player() { player_shader.end(); }

void PixelEffectShader::begin_object() {
    object_shader.begin();
    object_shader.setUniform1f("block_size", 10.0f);
    object_shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end_object() { object_shader.end(); }
