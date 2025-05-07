#include "warp_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

WarpEffectShader::WarpEffectShader() {
    _shader.load("shaders/player_effects/warp");
}

void WarpEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WarpEffectShader::end_player() { _shader.end(); }

void WarpEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WarpEffectShader::end_object() { _shader.end(); }
