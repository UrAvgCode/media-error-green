#pragma once

#include "effect_shader.h"

class WarpEffectShader : public EffectShader {
  public:
    WarpEffectShader();

    void begin_player() final;
    void end_player() final;

    void begin_object() final;
    void end_object() final;

  private:
    ofShader _shader;
};
