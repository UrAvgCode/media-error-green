#pragma once
#include <ofMath.h>
#include <ofMesh.h>
#include <ofVec2f.h>

#include <vector>

class Particle {
  public:
    const std::size_t max_trail_length = 50;
    const float max_speed = 4.0f;

    ofVec2f position;
    ofVec2f velocity;
    ofVec2f acceleration;

    Particle(float x, float y);

    void update();
    void draw();

    void apply_force(ofVec2f force);

    bool is_outside_of_screen() const;
    void apply_repulsion(const std::vector<Particle> &particles, float repulsion_radius, float repulsion_strength);

  protected:
    ofMesh mesh;

    void move_vertices();
    void wrap_position();
};
