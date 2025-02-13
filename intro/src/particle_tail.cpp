#include "particle_tail.h"

#include <algorithm>
#include <execution>
#include <functional>
#include <vector>

#include <ofGLProgrammableRenderer.h>

ParticleTail::ParticleTail(float x, float y) : position(ofVec2f(x, y)) {
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    auto &vertices = mesh.getVertices();
    auto &colors = mesh.getColors();

    vertices.resize(max_trail_length);
    colors.resize(max_trail_length);

    for (std::size_t i = 0; i < max_trail_length; ++i) {
        vertices[i] = static_cast<ofPoint>(position);

        float alpha = ofMap(static_cast<float>(i), 0, static_cast<float>(max_trail_length), 255, 0);
        colors[i] = ofColor(0, 255, 0, alpha);
    }
}

void ParticleTail::update_position(glm::vec2 position) {
    auto &vertices = mesh.getVertices();
    auto &colors = mesh.getColors();
    for (std::size_t i = vertices.size() - 1; i > 0; --i) {
        vertices[i] = vertices[i - 1];
    }

    vertices[0] = static_cast<ofPoint>(position);
}

void ParticleTail::draw() { mesh.draw(); }
