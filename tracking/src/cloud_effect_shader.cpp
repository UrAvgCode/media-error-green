#include "cloud_effect_shader.h"

#include <ofAppRunner.h>
#include <ofUtils.h>

CloudEffectShader::CloudEffectShader() : EffectShader("shaders/render/cloud") {
    _shader.load("shaders/effect/cloud");
}

void CloudEffectShader::begin_player() {}

void CloudEffectShader::end_player() {}

void CloudEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1i("time", ofGetElapsedTimeMillis());
    _shader.setUniform1f("min_grid_size", 4);
    _shader.setUniform1f("max_grid_size", 8);
    _shader.setUniform1f("speed", 0.003);
    _shader.setUniform1f("tolerance", 2);
}

void CloudEffectShader::end_object() { _shader.end(); }
