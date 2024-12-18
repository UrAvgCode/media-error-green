#pragma once
#include <ofMath.h>
#include <ofMesh.h>
#include <ofVec2f.h>
#include "ofMain.h"

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
    void apply_repulsion(vector<Particle> &particles, float repulsionRadius, float repulsionStrength);

protected:
    ofMesh mesh;

    void move_vertices();
    void wrap_position();
};
