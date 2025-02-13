#include "particle.h"

#include <algorithm>
#include <execution>
#include <functional>
#include <vector>

#include <ofGLProgrammableRenderer.h>

Particle::Particle(float x, float y) : position(ofVec2f(x, y)), velocity(ofVec2f(0, 0)), acceleration(ofVec2f(0, 0)) {
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    auto &vertices = mesh.getVertices();
    auto &colors = mesh.getColors();

    vertices.resize(max_trail_length);
    colors.resize(max_trail_length);
    on_logo_status.resize(max_trail_length, false); // Initialisiere den Status als "nicht auf Logo"

    for (std::size_t i = 0; i < max_trail_length; ++i) {
        vertices[i] = static_cast<ofPoint>(position);

        float alpha = ofMap(static_cast<float>(i), 0, static_cast<float>(max_trail_length), 255, 0);
        colors[i] = ofColor(0, 255, 0, alpha);
    }
}

void Particle::apply_force(glm::vec2 force) { acceleration += force; }

void Particle::update(const std::vector<std::pair<glm::vec2, glm::vec2>> &logo_vectors, float logo_tolerance) {
    velocity += acceleration;
    velocity.limit(max_speed);
    acceleration = {0, 0};

    position += velocity;

    // Prüfen, ob der Partikel auf einem Logo-Vektor ist
    is_on_logo = check_if_on_logo(logo_vectors, logo_tolerance);

    move_vertices();

    if (is_outside_of_screen()) {
        wrap_position();
    }
}

void Particle::draw() { mesh.draw(); }

void Particle::move_vertices() {
    auto &vertices = mesh.getVertices();
    auto &colors = mesh.getColors();
    for (std::size_t i = vertices.size() - 1; i > 0; --i) {
        vertices[i] = vertices[i - 1];
        on_logo_status[i] = on_logo_status[i - 1]; // Verschiebe den Logo-Status
    }

    vertices[0] = static_cast<ofPoint>(position);
    on_logo_status[0] = is_on_logo;

    // Farben aktualisieren
    for (std::size_t i = 0; i < vertices.size(); ++i) {
        float alpha = ofMap(static_cast<float>(i), 0, static_cast<float>(vertices.size()), 255, 0);
        if (on_logo_status[i]) {
            colors[i] = ofColor(0, 255, 0, alpha); // Weiß, wenn auf Logo
        } else {
            colors[i] = ofColor(0, 100, 0, alpha); // Grün sonst
        }
    }
}

void Particle::wrap_position() {
    const auto width = static_cast<float>(ofGetWidth());
    const auto height = static_cast<float>(ofGetHeight());

    if (position.x < 0) {
        position.x = width;
    } else if (position.x > width) {
        position.x = 0;
    }

    if (position.y < 0) {
        position.y = height;
    } else if (position.y > height) {
        position.y = 0;
    }

    for (auto &vertex: mesh.getVertices()) {
        vertex = static_cast<ofPoint>(position);
    }
}

bool Particle::is_outside_of_screen() const {
    const auto width = static_cast<float>(ofGetWidth());
    const auto height = static_cast<float>(ofGetHeight());

    for (std::size_t i = 0; i < mesh.getNumVertices(); ++i) {
        auto vertex = mesh.getVertex(i);
        if (vertex.x >= 0 && vertex.x <= width && vertex.y >= 0 && vertex.y <= height) {
            return false;
        }
    }
    return true;
}

void Particle::apply_repulsion(const std::vector<Particle> &particles, float repulsion_radius,
                               float repulsion_strength) {
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

void Particle::set_color(const ofColor &color) {
    auto &colors = mesh.getColors();
    for (std::size_t i = 0; i < colors.size(); ++i) {
        float alpha = colors[i].a; // Behalte den Alpha-Wert
        colors[i] = color;
        colors[i].a = alpha; // Setze Alpha wieder zurück
    }
}

// Prüft, ob sich der Partikel auf einem Logo-Vektor befindet
bool Particle::check_if_on_logo(const std::vector<std::pair<glm::vec2, glm::vec2>> &logo_vectors,
                                float logo_tolerance) {
    if (position.distance(logo_center) < logo_radius) {
        for (const auto &logo_vec: logo_vectors) {
            if (position.distance(logo_vec.first) < logo_tolerance) {
                return true;
            }
        }
    }
    return false;
}
