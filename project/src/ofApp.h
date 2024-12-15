#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxSvg.h"


class ofApp : public ofBaseApp{

	public:
		int screen_width = ofGetWidth();
		int screen_height = ofGetHeight();

		//Flow Field
		int cols, rows; // count of rows an colums in gitter
		int resolution = 20; // size of every cell
		vector<ofVec2f> flowField; 
		float zOffset; // for animated Perlin noise

		//particles
		vector<Particle> particles;
		int num_particles = 1000;
		float repulsion_radius = 10;
		float repulsion_strength = 5;

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
		void create_circle_vectors();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
