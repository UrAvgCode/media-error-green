#pragma once

#include "effect_shader.h"

class GlitchEffectShader : public EffectShader {
  public:
    GlitchEffectShader();

    void begin_player() override;

    void end_player() override;

    void begin_object() override;

    void end_object() override;

  private:
    ofShader shader;
};
