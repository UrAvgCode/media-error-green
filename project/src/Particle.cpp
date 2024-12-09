#include "Particle.h"
#include <ofGLProgrammableRenderer.cpp>

void Particle::applyForce(ofVec2f force) {
    acceleration += force;
}

void Particle::update() {

    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    acceleration *= 0; // Beschleunigung zurücksetzen

    trail.push_back(position);
    if (trail.size() > max_trail_length) {
        trail.erase(trail.begin());
    }
}

void Particle::edges() {
    if (position.x > ofGetWidth()) {
        ofDrawLine(position, ofVec2f(0, position.y));
        position.x = 0;
    }
    if (position.x < 0) {
        ofDrawLine(position, ofVec2f(ofGetWidth(), position.y));
        position.x = ofGetWidth();
    }
    if (position.y > ofGetHeight()) {
        ofDrawLine(position, ofVec2f(position.x, 0));
        position.y = 0;
    }
    if (position.y < 0) {
        ofDrawLine(position, ofVec2f(position.x, ofGetHeight()));
        position.y = ofGetHeight();
    }
}

void Particle::draw() {
   // ofNoFill(); // Keine Füllung

    // Setze die Farbe mit Transparenz (Alpha-Wert für Fading-Effekt)
    for (int i = 0; i < trail.size(); ++i) {
        float alpha = ofMap(i, 0, trail.size(), 255, 0);  // Alpha-Wert nimmt ab
        ofSetColor(0, 255, 0, alpha);  // green with variating opacity

        ofDrawCircle(trail[i], 1);  // Zeichne an jeder Position einen kleinen Kreis
    }
}

