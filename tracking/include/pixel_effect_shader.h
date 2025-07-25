#pragma once

#include "effect_shader.h"

class PixelEffectShader : public EffectShader {
  public:
    PixelEffectShader();

    void begin_player() final;
    void end_player() final;

    void begin_object() final;
    void end_object() final;

  private:
    ofShader _shader;
};
