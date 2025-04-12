#pragma once

#include <ofShader.h>

class EffectShader {
  public:
    virtual ~EffectShader() = default;

    virtual void begin() = 0;

    virtual void end() = 0;

protected:
    ofShader shader;
};
