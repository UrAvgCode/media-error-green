#include "Particle.h"
#include <ofGLProgrammableRenderer.cpp>

void Particle::applyForce(ofVec2f force) {
    acceleration += force;
}

void Particle::update() {

    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    acceleration *= 0; // undo acceleration

    // adds one particle to trail each update
    trail.push_back(position);
    if (trail.size() > max_trail_length) {
        trail.erase(trail.begin());
    }
}

//checks if particle is still in screen
void Particle::check_for_edges() {
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

    // put color with transparency
    for (int i = 0; i < trail.size(); ++i) {
        float alpha = ofMap(i, 0, trail.size(), 0, 150); // newest particle has highest opacity
        ofSetColor(0, 255, 0, alpha);  // green with variating opacity

        ofDrawCircle(trail[i], 5);  // draw on each position a circle
    }
}

