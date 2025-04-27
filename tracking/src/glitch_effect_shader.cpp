#include "glitch_effect_shader.h"

#include <ofUtils.h>
#include <ofAppRunner.h>

GlitchEffectShader::GlitchEffectShader() {
    shader.load("shaders/player_effects/glitch");
}

void GlitchEffectShader::begin_player() {
    shader.begin();
    shader.setUniform1f("aberration_amount", 5);
    shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
    shader.setUniform1f("rand1", ofRandomHeight());
    shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_player() { shader.end(); }

void GlitchEffectShader::begin_object() {
    shader.begin();
    shader.setUniform1f("aberration_amount", 5);
    shader.setUniform1f("time", static_cast<float>(ofGetElapsedTimeMillis()) / 50.0f);
    shader.setUniform1f("rand1", ofRandomHeight());
    shader.setUniform1f("rand2", ofRandomHeight());
}

void GlitchEffectShader::end_object() { shader.end(); }
