#include "glitch_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

GlitchEffectShader::GlitchEffectShader() {
    _shader.load("shaders/effect/glitch");
}

void GlitchEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("rand1", ofRandomHeight());
    _shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_player() { _shader.end(); }

void GlitchEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("rand1", ofRandomHeight());
    _shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_object() { _shader.end(); }
