#include "glitch_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

GlitchEffectShader::GlitchEffectShader() {
    shader.load("shaders/player_effects/glitch");
}

void GlitchEffectShader::begin_player() {
    shader.begin();
    shader.setUniform1f("rand1", ofRandomHeight());
    shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_player() { shader.end(); }

void GlitchEffectShader::begin_object() {
    shader.begin();
    shader.setUniform1f("rand1", ofRandomHeight());
    shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_object() { shader.end(); }
