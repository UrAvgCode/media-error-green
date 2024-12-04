#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	resolution = 20; // Zellenbreite und -höhe
	cols = ofGetWidth() / resolution;
	rows = ofGetHeight() / resolution;
	ofBackground(0, 0, 0);
	zOffset = 0.0;

	quality = OF_IMAGE_QUALITY_WORST;
	maxSize = 2048;
	glitchStart = .6;
	reset();

	flowField.resize(cols * rows); // Initialisiere das Vektorfeld

	for (int i = 0; i < cols * rows; i++) {
		// Zufällige Vektoren für den Start
		flowField[i] = ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1));
	}

	for (int i = 0; i < 10; i++) {
		particles.push_back(Particle());
	}
}

//--------------------------------------------------------------
void ofApp::reset() {
	generation = 0;
	img.grabScreen(0,0,cols, rows);
	// pick random for bleeding
	addX = ofRandom(0, 16);
	addY = ofRandom(0, 16);
	subX = ofRandom(0, addX);
	subY = ofRandom(0, addY);
}

//--------------------------------------------------------------
void ofApp::update(){

	string curFilename = "compressed.jpg";

	int size = img.getWidth();

	// keeps the image from getting too big
	if (size < maxSize) {
		img.save(curFilename, quality);

		if (ofGetKeyPressed('g')) {
			// this portion glitches the jpeg file
			// first loading the file (as binary)
			ofBuffer file = ofBufferFromFile(curFilename, true);
			int fileSize = file.size();
			char* buffer = file.getData();

			// pick a byte offset that is somewhere near the end of the file
			int whichByte = (int)ofRandom(fileSize * glitchStart, fileSize);
			// and pick a bit in that byte to turn on
			int whichBit = ofRandom(8);
			char bitMask = 1 << whichBit;
			// using the OR operator |, if the bit isn't already on this will turn it on
			buffer[whichByte] |= bitMask;

			// write the file out like nothing happened
			ofBufferToFile(curFilename, file, true);
			img.load(curFilename);
		}
		else {
			img.load(curFilename);

			// this if switches every other frame
			// resizing up and down breaks the 8x8 JPEG blocks
			if (ofGetFrameNum() % 2 == 0) {
				// resize a little bigger
				img.resize(size + addX, size + addY);
			}
			else {
				// then resize a little smaller
				img.resize(size - subX, size - subY);
			}
		}
		generation++;
	}

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

		if (p.position.x < 0) { p.position.x = cols*20; }
		if (p.position.x > cols*20) { p.position.x = 0; }
		if (p.position.y < 0) { p.position.y = rows * 20; }
		if (p.position.y > rows * 20) { p.position.y = 0; }


		p.update();
		p.edges();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Flow Field visualisieren
	//for (int y = 0; y < rows; y++) {
	//	for (int x = 0; x < cols; x++) {
	//		ofVec2f vec = flowField[y * cols + x];
	//		ofPushMatrix();
	//		ofTranslate(x * resolution, y * resolution);
	//		ofDrawLine(0, 0, vec.x * resolution, vec.y * resolution); // Richtung zeichnen
	//		ofPopMatrix();
	//	}
	//}

	ofSetColor(255);
	img.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofSetColor(0);
	ofDrawRectangle(5, 5, 290, 45);
	ofSetColor(255);
	ofDrawBitmapString("Currently on generation " + ofToString(generation), 10, 20);
	ofDrawBitmapString("Click to reset, hold 'g' to glitch.", 10, 40);

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
