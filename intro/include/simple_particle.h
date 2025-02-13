#pragma once

#include <vector>

#include <ofMath.h>
#include <ofMesh.h>
#include <ofVec2f.h>

class SimpleParticle {
  public:
    glm::vec2 position;
    glm::vec2 acceleration;
    glm::vec2 velocity;

    SimpleParticle(glm::vec2 pos) : position(pos) {}
};
