#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxSvg.h"


class ofApp : public ofBaseApp{

	public:
		int screen_width = ofGetWidth();
		int screen_height = ofGetHeight();

		//Flow Field
		int cols, rows; // Anzahl der Spalten und Reihen im Gitter
		int resolution = 20; // size of every cell
		vector<ofVec2f> flowField; // Array für die Vektoren im Gitter
		float zOffset; // Für animierten Perlin Noise

		//particles
		vector<Particle> particles;
		int num_particles = 1000;

		//logo
		ofImage logo;
		ofxSVG logo_svg;

		//vector<bool> logoMask;   // masc of logo
		vector<pair <ofVec2f, ofVec2f>> logo_vectors; // 
		int logo_left, logo_right, logo_top, logo_bottom;
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
