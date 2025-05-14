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

    virtual void draw_player(const ofTexture &depth_tex, const ofTexture &body_index_tex,
                             const ofTexture &depth_to_world_tex, const std::vector<int> &body_ids, ofEasyCam *camera,
                             int id) override;

  private:
    ofShader _shader;
    ofImage _matrix;
    ofImage _matrix_logo;
};
