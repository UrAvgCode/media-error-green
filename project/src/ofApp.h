#pragma once

#include "ofMain.h"
#include "../Particle.h"


class ofApp : public ofBaseApp{

	public:
		int cols, rows; // Anzahl der Spalten und Reihen im Gitter
		int resolution; // Größe jeder Zelle
		vector<ofVec2f> flowField; // Array für die Vektoren im Gitter
		vector<Particle> particles;
		float zOffset; // Für animierten Perlin Noise

		void setup();
		void update();
		void draw();

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
