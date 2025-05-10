#pragma once

#include <k4abttypes.h>

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofTexture.h>
#include <ofVbo.h>

class EffectShader {
  public:
    EffectShader();
    EffectShader(std::string);
    EffectShader(std::string, std::string);

    virtual void begin_player();

    virtual void end_player();

    virtual void begin_object();

    virtual void end_object();

    virtual void draw_player(ofTexture depth_tex, ofTexture body_index_tex, ofTexture depth_to_world_tex,
                             std::vector<int> &body_ids, ofEasyCam *camera, int id);

  protected:
    ofShader _render_shader;
    ofVbo _player_vbo;
};
