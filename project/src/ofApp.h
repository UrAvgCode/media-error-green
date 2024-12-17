#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxSvg.h"


class ofApp : public ofBaseApp {
public:
	int screen_width = ofGetWidth();
	int screen_height = ofGetHeight();

	//Flow Field
	const int resolution = 20; // size of every cell
	const int cols = ofGetWidth() / resolution;
	const int rows = ofGetHeight() / resolution;

	vector<ofVec2f> flowField;
	float zOffset; // for animated Perlin noise

	//particles
	vector<Particle> particles;
	const int num_particles = 2000;
	const float repulsion_radius = 10;
	const float repulsion_strength = 5;

	const float attraction_radius = 20;
	const float attraction_strength = 1000;

	//logo
	ofxSVG logo_svg;
	float logo_scale = 1.0;
	string image = "logo_lines4.svg";
	vector<pair <ofVec2f, ofVec2f>> logo_vectors;
	int logo_left, logo_right, logo_top, logo_bottom;

	//circle
	vector<pair <ofVec2f, ofVec2f>> circle_vectors;;

	void setup();
	void update();
	void draw();

	void create_logo_vectors();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
};
