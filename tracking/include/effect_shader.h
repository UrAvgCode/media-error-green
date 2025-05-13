#pragma once

#include <string>
#include <vector>

#include <k4abttypes.h>

#include <ofEasyCam.h>
#include <ofShader.h>
#include <ofTexture.h>
#include <ofVbo.h>

class EffectShader {
  public:
    explicit EffectShader();
    explicit EffectShader(const std::string &);
    explicit EffectShader(const std::string &, const std::string &);

    virtual void begin_player();
    virtual void end_player();

    virtual void begin_object();
    virtual void end_object();

    virtual void draw_player(const ofTexture &depth_tex, const ofTexture &body_index_tex,
                             const ofTexture &depth_to_world_tex, const std::vector<int> &body_ids, ofEasyCam *camera,
                             int id);

  protected:
    ofShader _render_shader;
    ofVbo _player_vbo;
};
