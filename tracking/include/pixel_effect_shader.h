#pragma once

#include "effect_shader.h"

class PixelEffectShader : public EffectShader {
  public:
    PixelEffectShader();

    void begin() override;

    void end() override;
};
