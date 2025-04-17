#pragma once

#include <ofShader.h>

class EffectShader {
  public:
    EffectShader();

    virtual void begin_player();

    virtual void end_player();

    virtual void begin_object();

    virtual void end_object();
};
