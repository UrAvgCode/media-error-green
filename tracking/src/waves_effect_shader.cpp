#include "waves_effect_shader.h"

#include <ofAppRunner.h>
#include <ofUtils.h>

WavesEffectShader::WavesEffectShader() : EffectShader("shaders/player_effects/waves_renderer.vert", "shaders/player_effects/waves_renderer.frag") {
    _shader.load("shaders/player_effects/waves");
}

void WavesEffectShader::begin_player() {}

void WavesEffectShader::end_player() {}

void WavesEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WavesEffectShader::end_object() { _shader.end(); }
