#pragma once
#include "ofMain.h"
#include <ofMath.h>
#include <ofVec2f.h>

class Particle {
public:
    ofVec2f position, velocity, acceleration;
    float maxSpeed;
    vector<ofVec2f> trail; //history of positions
    int max_trail_length = 50;

    Particle(float x, float y) {
        position = ofVec2f(x, y);
        velocity = ofVec2f(0, 0);
        acceleration = ofVec2f(0, 0);
        maxSpeed = 4;
    }

    void applyForce(ofVec2f force);
    void update();
    void edges();
    void draw();

};
