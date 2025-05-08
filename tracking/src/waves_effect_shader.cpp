#include "waves_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

WavesEffectShader::WavesEffectShader() {
    _shader.load("shaders/player_effects/waves");
}

void WavesEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WavesEffectShader::end_player() { _shader.end(); }

void WavesEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WavesEffectShader::end_object() { _shader.end(); }
