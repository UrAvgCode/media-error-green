#pragma once

#include "effect_shader.h"

class ChromaticEffectShader : public EffectShader {
  public:
    ChromaticEffectShader();

    void begin_player() override;

    void end_player() override;

    void begin_object() override;

    void end_object() override;

  private:
    ofShader _shader;
};
