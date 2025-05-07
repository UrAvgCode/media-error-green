#pragma once

#include "effect_shader.h"

class PixelEffectShader : public EffectShader {
  public:
    PixelEffectShader();

    void begin_player() override;

    void end_player() override;

    void begin_object() override;

    void end_object() override;

  private:
    ofShader _player_shader;
    ofShader _object_shader;
};
