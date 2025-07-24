#pragma once

#include "effect_shader.h"

#include <vector>

#include <ofGraphics.h>
#include <ofImage.h>
#include <ofPixels.h>
#include <ofShader.h>

class MatrixEffectShader : public EffectShader {
  public:
    MatrixEffectShader();

    void begin_player() final;
    void end_player() final;

    void begin_object() final;
    void end_object() final;

    void draw_player(const ofTexture &depth_tex, const ofTexture &body_index_tex, const ofTexture &depth_to_world_tex,
                     const std::vector<int> &body_ids, ofEasyCam *camera, int id) final;

  private:
    ofShader _shader;
    ofImage _matrix;
    ofImage _matrix_logo;
};
