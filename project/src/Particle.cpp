#include "Particle.h"
#include <ofGLProgrammableRenderer.cpp>

Particle::Particle(float x, float y) :
	position(ofVec2f(x, y)),
	velocity(ofVec2f(0, 0)),
	acceleration(ofVec2f(0, 0))
{
	mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	for (std::size_t i = 0; i < max_trail_length; ++i) {
		mesh.addVertex(ofPoint(x, y));

		float alpha = 255.0f - (255.0f / max_trail_length) * i;
		mesh.addColor(ofColor(0, 255, 0, alpha));
	}
}

void Particle::apply_force(ofVec2f force) {
	acceleration += force;
}

void Particle::update() {
	velocity += acceleration;
	velocity.limit(max_speed);
	position += velocity;
	acceleration *= 0; // undo acceleration

	for (std::size_t i = mesh.getNumVertices() - 1; i > 0; --i) {
		mesh.setVertex(i, mesh.getVertex(i - 1));
	}

	mesh.setVertex(0, static_cast<ofPoint>(position));

	if (is_outside_of_screen()) {
		if (position.x < 0) position.x = ofGetWidth();
		if (position.x > ofGetWidth()) position.x = 0;
		if (position.y < 0) position.y = ofGetHeight();
		if (position.y > ofGetHeight()) position.y = 0;

		for (std::size_t i = 0; i < mesh.getNumVertices(); ++i) {
			auto vertex = mesh.getVertex(i);
			if (vertex.x < 0) vertex.x = ofGetWidth();
			if (vertex.x > ofGetWidth()) vertex.x = 0;
			if (vertex.y < 0) vertex.y = ofGetHeight();
			if (vertex.y > ofGetHeight()) vertex.y = 0;
			mesh.setVertex(i, vertex);
		}
	}
}

void Particle::draw() {
	mesh.draw();
}

bool Particle::is_outside_of_screen() const {
	for (std::size_t i = 0; i < mesh.getNumVertices(); ++i) {
		auto vertex = mesh.getVertex(i);
		if (vertex.x >= 0 && vertex.x <= ofGetWidth() &&
			vertex.y >= 0 && vertex.y <= ofGetHeight()) {
			return false;
		}
	}
	return true;
}
