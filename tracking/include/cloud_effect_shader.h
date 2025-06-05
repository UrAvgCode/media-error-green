#pragma once

#include "effect_shader.h"

class CloudEffectShader : public EffectShader {
  public:
    CloudEffectShader();

    void begin_player() final;
    void end_player() final;

    void begin_object() final;
    void end_object() final;

  private:
    ofShader _shader;
};
