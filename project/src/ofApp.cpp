#include "ofApp.h"

void ofApp::setup() {
    ofSetBackgroundColor(20, 20, 20);
    ofSetFrameRate(60);

    fboBlur.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

    for (int i = 0; i < 10; i++) {
        ofPoint pos = ofPoint(ofRandom(100, ofGetWidth() - 100), ofRandom(100, ofGetHeight() - 100));
        ofPoint vel = ofPoint(ofRandom(-2, 2), ofRandom(-2, 2));
        float radius = ofRandom(20, 50);
        circles.push_back(Circle(pos, vel, radius));
    }
}

void ofApp::update() {
    for (auto& circle : circles) {
        circle.update();
    }
}

void ofApp::draw() {
    fboBlur.begin();
    ofClear(0, 0, 0, 0);
    for (auto& circle : circles) {
        ofSetColor(255, 100, 150, 200);
        ofDrawCircle(circle.position, circle.radius);
    }
    fboBlur.end();

    fboBlur.draw(0, 0); // FBO direkt zeichnen
}
