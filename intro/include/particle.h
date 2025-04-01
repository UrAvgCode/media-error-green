#pragma once

#include <ofVec2f.h>

struct Particle {
    glm::vec2 velocity;
    std::array<glm::vec2, 100> positions;
};
