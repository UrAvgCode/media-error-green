#pragma once

#include <ofMain.h>

class Scene {
  public:
    virtual ~Scene() = default;

    virtual void update() = 0;
    virtual void draw() = 0;
};
