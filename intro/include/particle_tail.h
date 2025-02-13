#pragma once

#include <vector>

#include <ofMath.h>
#include <ofMesh.h>
#include <ofVec2f.h>

#include <ofMain.h>

class ParticleTail {
  public:
    const std::size_t max_trail_length = 100;
    
    ParticleTail(float x, float y);
    void update_position(glm::vec2 position);
    void draw();

  protected:
    glm::vec2 position;
    ofMesh mesh;
};
