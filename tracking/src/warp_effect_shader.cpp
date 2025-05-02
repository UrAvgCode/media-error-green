#include "warp_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

WarpEffectShader::WarpEffectShader() {
    shader.load("shaders/player_effects/warp");
}

void WarpEffectShader::begin_player() {
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WarpEffectShader::end_player() { shader.end(); }

void WarpEffectShader::begin_object() {
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WarpEffectShader::end_object() { shader.end(); }
