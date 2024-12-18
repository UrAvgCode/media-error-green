#include "Particle.h"

#include <ofGLProgrammableRenderer.h>

#include <algorithm>
#include <execution>

Particle::Particle(float x, float y) : position(ofVec2f(x, y)), velocity(ofVec2f(0, 0)), acceleration(ofVec2f(0, 0)) {
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    auto &vertices = mesh.getVertices();
    auto &colors = mesh.getColors();

    vertices.resize(max_trail_length);
    colors.resize(max_trail_length);

    for (std::size_t i = 0; i < max_trail_length; ++i) {
        vertices[i] = static_cast<ofPoint>(position);

        float alpha = 255.0f - (255.0f / max_trail_length) * i;
        colors[i] = ofColor(0, 255, 0, alpha);
    }
}

void Particle::apply_force(ofVec2f force) { acceleration += force; }

void Particle::update() {
    velocity += acceleration;
    velocity.limit(max_speed);
    acceleration = {0, 0};

    position += velocity;
    move_vertices();

    if (is_outside_of_screen()) {
        wrap_position();
    }
}

void Particle::draw() { mesh.draw(); }

void Particle::move_vertices() {
    auto &vertices = mesh.getVertices();
    for (std::size_t i = vertices.size() - 1; i > 0; --i) {
        vertices[i] = vertices[i - 1];
    }

    vertices[0] = static_cast<ofPoint>(position);
}

void Particle::wrap_position() {
    if (position.x < 0)
        position.x = ofGetWidth();
    if (position.x > ofGetWidth())
        position.x = 0;
    if (position.y < 0)
        position.y = ofGetHeight();
    if (position.y > ofGetHeight())
        position.y = 0;

    for (auto &vertex: mesh.getVertices()) {
        vertex = static_cast<ofPoint>(position);
    }
}

bool Particle::is_outside_of_screen() const {
    for (std::size_t i = 0; i < mesh.getNumVertices(); ++i) {
        auto vertex = mesh.getVertex(i);
        if (vertex.x >= 0 && vertex.x <= ofGetWidth() && vertex.y >= 0 && vertex.y <= ofGetHeight()) {
            return false;
        }
    }
    return true;
}

void Particle::apply_repulsion(std::vector<Particle> &particles, float repulsion_radius, float repulsion_strength) {
    acceleration += std::transform_reduce(
            std::execution::par_unseq, particles.begin(), particles.end(), ofVec2f(), std::plus<>(), [&](auto &other) {
                if (&other == this) {
                    return ofVec2f(0, 0);
                }

                ofVec2f diff = position - other.position;
                float distance = diff.length();

                if (distance > repulsion_radius) {
                    return ofVec2f(0, 0);
                }

                diff.normalize();
                float force =
                        ofMap(distance, 0, repulsion_radius, repulsion_strength, 0); // Kraft nimmt mit Entfernung ab
                return diff * force;
            });
}
