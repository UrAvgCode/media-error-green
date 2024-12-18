#pragma once

#include "ofMain.h"

class Circle {
public:
    ofPoint position;
    ofPoint velocity;
    float radius;

    Circle(ofPoint pos, ofPoint vel, float r)
        : position(pos), velocity(vel), radius(r) {
    }

    void update() {
        position += velocity;
        if (position.x < radius || position.x > ofGetWidth() - radius) {
            velocity.x *= -1;
        }
        if (position.y < radius || position.y > ofGetHeight() - radius) {
            velocity.y *= -1;
        }
    }
};

class ofApp : public ofBaseApp {
public:
    vector<Circle> circles;
    ofFbo fboBlur;

    void setup();
    void update();
    void draw();
};
