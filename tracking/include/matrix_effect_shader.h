#pragma once

#include "effect_shader.h"

#include <ofGraphics.h>
#include <ofImage.h>
#include <ofShader.h>
#include <ofPixels.h>

class MatrixEffectShader : public EffectShader {
  public:
    MatrixEffectShader();

    void begin_player() override;
    void end_player() override;

    void begin_object() override;
    void end_object() override;

  private:
    ofShader _shader;
    ofImage _matrix;
    ofImage _matrix_logo;
};
