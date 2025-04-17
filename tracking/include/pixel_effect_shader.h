#pragma once

#include "effect_shader.h"

class PixelEffectShader : public EffectShader {
  public:
    PixelEffectShader();

    void begin_player() override;

    void end_player() override;
};
