#include "chromatic_effect_shader.h"

#include <ofAppRunner.h>
#include <ofUtils.h>

ChromaticEffectShader::ChromaticEffectShader() { _shader.load("shaders/effect/chromatic"); }

void ChromaticEffectShader::begin_player() {
    _shader.begin();
    _shader.setUniform1f("aberration_amount", 10.0f);
}

void ChromaticEffectShader::end_player() { _shader.end(); }

void ChromaticEffectShader::begin_object() {
    _shader.begin();
    _shader.setUniform1f("aberration_amount", 3.0f);
}

void ChromaticEffectShader::end_object() { _shader.end(); }
