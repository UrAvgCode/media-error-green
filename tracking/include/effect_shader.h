#pragma once

#include <ofShader.h>

class EffectShader {
  public:
    EffectShader();

    virtual void begin();

    virtual void end();

protected:
    ofShader shader;
};
