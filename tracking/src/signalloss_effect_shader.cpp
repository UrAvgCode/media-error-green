#include "signalloss_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

SignallossEffectShader::SignallossEffectShader() {
    shader.load("shaders/player_effects/signalloss");
}

void SignallossEffectShader::begin_player() {
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void SignallossEffectShader::end_player() { shader.end(); }

void SignallossEffectShader::begin_object() {
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void SignallossEffectShader::end_object() { shader.end(); }
