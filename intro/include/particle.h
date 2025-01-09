#pragma once

#include <vector>

#include <ofMath.h>
#include <ofMesh.h>
#include <ofVec2f.h>

class Particle {
  public:
    static inline ofVec2f logo_center{};
    static inline float logo_radius{};

    const std::size_t max_trail_length = 100;
    const float max_speed = 4.0f;

    bool is_on_logo = false; // Gibt an, ob sich der Partikel auf einem Logo-Vektor befindet
    std::vector<bool> on_logo_status; // Status für jeden Vertex

    ofVec2f position;
    ofVec2f velocity;
    ofVec2f acceleration;

    Particle(float x, float y);

    void update(const std::vector<std::pair<ofVec2f, ofVec2f>> &logo_vectors, float logo_tolerance);
    void draw();

    void apply_force(ofVec2f force);

    bool is_outside_of_screen() const;
    void apply_repulsion(const std::vector<Particle> &particles, float repulsion_radius, float repulsion_strength);
    bool check_if_on_logo(const std::vector<std::pair<ofVec2f, ofVec2f>> &logo_vectors, float logo_tolerance);


  protected:
    ofMesh mesh;

    void move_vertices();
    void wrap_position();
    void Particle::set_color(const ofColor &color);
};
