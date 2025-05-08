#pragma once

#include "effect_shader.h"

class WarpEffectShader : public EffectShader {
  public:
    WarpEffectShader();

    void begin_player() override;

    void end_player() override;

    void begin_object() override;

    void end_object() override;

  private:
    ofShader _shader;
};
