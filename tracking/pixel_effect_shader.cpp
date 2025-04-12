#include "pixel_effect_shader.h"

PixelEffectShader::PixelEffectShader() { shader.load("shaders/pixel"); }

void PixelEffectShader::begin() { 
	shader.begin();
    shader.setUniform1f("block_size", 10.0f);
    shader.setUniform1f("quality", 0.5f);
}

void PixelEffectShader::end() { shader.end(); }
