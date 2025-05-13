#include "warp_effect_shader.h"

#include <ofAppRunner.h>
#include <ofUtils.h>

WarpEffectShader::WarpEffectShader() : EffectShader("shaders/render/warp") {
    _shader.load("shaders/effect/warp");
}

void WarpEffectShader::begin_player() {}

void WarpEffectShader::end_player() {}

void WarpEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("time", ofGetElapsedTimeMillis());
}

void WarpEffectShader::end_object() { _shader.end(); }
