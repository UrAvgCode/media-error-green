#include "Particle.h"
#include <ofGLProgrammableRenderer.cpp>

void Particle::applyForce(ofVec2f force) {
    acceleration += 5;
}

void Particle::update() {

    velocity = acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    acceleration *= 0; // Beschleunigung zurücksetzen
}

void Particle::edges() {
    if (position.x > ofGetWidth()) position.x = 0;
    if (position.x < 0) position.x = ofGetWidth();
    if (position.y > ofGetHeight()) position.y = 0;
    if (position.y < 0) position.y = ofGetHeight();
}

void Particle::draw() {
    ofDrawCircle(position, 25);
}

