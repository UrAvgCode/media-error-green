#pragma once

#include "ofMain.h"
#include "ofxSvg.h"

#include "Particle.h"

#include <vector>

class ofApp : public ofBaseApp {
public:
    const int screen_width = ofGetWidth();
    const int screen_height = ofGetHeight();

    // Flow Field
    const int resolution = 20; // size of every cell
    const int cols = ofGetWidth() / resolution;
    const int rows = ofGetHeight() / resolution;

    std::vector<ofVec2f> flow_field;
    float z_offset; // for animated Perlin noise

    // particles
    std::vector<Particle> particles;
    const int num_particles = 2000;
    const float repulsion_radius = 10;
    const float repulsion_strength = 5;

    const float attraction_radius = 20;
    const float attraction_strength = 1000;

    // logo
    ofxSVG logo_svg;
    const float logo_scale = 1.0;
    string image = "logo_lines5.svg";
    std::vector<pair<ofVec2f, ofVec2f>> logo_vectors;
    int logo_left, logo_right, logo_top, logo_bottom;

    const ofVec2f logo_position = {ofGetWidth() / 2, ofGetHeight() / 2};
    const float logo_width = logo_svg.getWidth() * logo_scale;
    const float logo_height = logo_svg.getHeight() * logo_scale;

    void setup() override;
    void update() override;
    void draw() override;

    void create_logo_vectors();

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;
};
