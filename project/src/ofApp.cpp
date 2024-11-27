#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	resolution = 20; // Zellenbreite und -höhe
	cols = ofGetWidth() / resolution;
	rows = ofGetHeight() / resolution;
	zOffset = 0.0;


	flowField.resize(cols * rows); // Initialisiere das Vektorfeld

	for (int i = 0; i < cols * rows; i++) {
		// Zufällige Vektoren für den Start
		flowField[i] = ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1));
	}

	for (int i = 0; i < 1000; i++) {
		particles.push_back(Particle());
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	zOffset += 0.01; // Animationsversatz erhöhen

	// Vektoren im Flow Field basierend auf Perlin Noise berechnen
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			float angle = ofNoise(x * 0.1, y * 0.1, zOffset) * TWO_PI; // Noise erzeugt Winkel
			flowField[y * cols + x] = ofVec2f(cos(angle), sin(angle));
		}
	}

	// Partikel aktualisieren
	for (auto& p : particles) {
		int xIndex = floor(p.position.x / resolution);
		int yIndex = floor(p.position.y / resolution);
		int index = yIndex * cols + xIndex;

		if (index >= 0 && index < flowField.size()) {
			ofVec2f force = flowField[index];
			p.applyForce(force);
		}

		p.update();
		p.edges();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Flow Field visualisieren
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			ofVec2f vec = flowField[y * cols + x];
			ofPushMatrix();
			ofTranslate(x * resolution, y * resolution);
			ofDrawLine(0, 0, vec.x * resolution, vec.y * resolution); // Richtung zeichnen
			ofPopMatrix();
		}
	}

	// Partikel zeichnen
	for (auto& p : particles) {
		p.draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
