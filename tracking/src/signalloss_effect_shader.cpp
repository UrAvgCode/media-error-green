#include "signalloss_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

SignallossEffectShader::SignallossEffectShader() {
    _shader.load("shaders/player_effects/signalloss");
}

void SignallossEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("block_size", 8.0f);
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void SignallossEffectShader::end_player() { _shader.end(); }

void SignallossEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("block_size", 4.0f);
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void SignallossEffectShader::end_object() { _shader.end(); }
