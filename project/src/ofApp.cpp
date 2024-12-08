#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	cols = ofGetWidth() / resolution;
	rows = ofGetHeight() / resolution;
	flowField.resize(cols * rows); // Initialisiere das Vektorfeld
	zOffset = 0.0;

	// creating logo masc
	logo.load("rotateGreen.png");
	logo.resize(200, 200);
	logo_left = screen_width / 2 - logo.getWidth() / 2;
	logo_right = screen_width / 2 + logo.getWidth() / 2;
	logo_top = screen_height / 2 - logo.getHeight() / 2;
	logo_bottom = screen_height / 2 + logo.getHeight() / 2;
	create_logo_mask();
	

	//for (int i = 0; i < cols * rows; i++) {
	//	// Zufällige Vektoren für den Start
	//	flowField[i] = ofVec2f(ofRandom(-1, 1), ofRandom(-1, 1));
	//}

	for (int i = 0; i < num_particles; i++) {
		//particles.push_back(Particle());
		particles.push_back(Particle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight())));

	}
}

//--------------------------------------------------------------
void ofApp::update(){
	zOffset += 0.01; // Animationsversatz erhöhen

	// Vektoren im Flow Field basierend auf Perlin Noise berechnen
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			ofVec2f currentPos(x,y);
			if (std::find(logoPositions.begin(), logoPositions.end(), currentPos) != logoPositions.end()) {
				continue;
			}
			else {
				float angle = ofNoise(x * 0.1, y * 0.1, zOffset) * TWO_PI; // Noise erzeugt Winkel
				flowField[y * cols + x] = ofVec2f(cos(angle), sin(angle));
			}


			
		}
	}

	// Partikel aktualisieren
	for (auto& p : particles) {
		int xIndex = floor(p.position.x / resolution);
		int yIndex = floor(p.position.y / resolution);
		int index = yIndex * cols + xIndex;

		// using perlin noise as force
		if (index >= 0 && index < flowField.size()) {
			ofVec2f force = flowField[index];
			p.applyForce(force);
		}

		// calculate logo force
		float attractionRadius = 50; // Radius, in dem Partikel vom Logo angezogen werden
		float attractionStrength = 100; // Stärke der Anziehungskraft

		// using logo force if particles are close
		for (auto& logoPos : logoPositions) {
			float distance = p.position.distance(logoPos);
			if (distance < attractionRadius) {
				ofVec2f attractionForce = (logoPos - p.position).normalized() * attractionStrength;
				p.applyForce(attractionForce);
			}
		}

		p.update();
		p.edges();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	//logo.draw(logo_left, logo_top);


	// Flow Field visualisieren
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			////current position on screen
			//int posX = x * resolution;
			//int posY = y * resolution;

			//// Überprüfen, ob der aktuelle Punkt im Logo-Bereich liegt
			//if (posX >= logo_left && posX < logo_right &&
			//	posY >= logo_top && posY < logo_bottom) {

			//	// Umrechnung der Fensterkoordinaten in Logo-Koordinaten
			//	int logoX = posX - logo_left;
			//	int logoY = posY - logo_top;

			//	// Wenn die Maske an dieser Position "true" ist, wird der Vektor übersprungen
			//	if (logoMask[logoY * logo.getWidth() + logoX]) {
			//		continue;
			//	}
			//}

			//ofVec2f vec = flowField[y * cols + x];
			//ofPushMatrix();
			//ofTranslate(posX, posY);
			//ofDrawLine(0, 0, vec.x * resolution, vec.y * resolution); // Richtung zeichnen
			//ofPopMatrix();

			ofVec2f vec = flowField[y * cols + x];
			ofPushMatrix();
			ofTranslate(x * resolution, y * resolution);
			ofDrawLine(0, 0, vec.x * resolution * 0.5, vec.y * resolution * 0.5);
			ofPopMatrix();
		}
	}

	// Partikel zeichnen
	for (auto& p : particles) {
		p.draw();
	}

	// Logo-Vektoren zeichnen
	ofSetColor(0, 255, 0); // Rote Linien für das Logo
	for (auto& pos : logoPositions) {
		//ofDrawLine(pos.x, pos.y, pos.x +1, pos.y + 1);
	}
}

void ofApp::create_logo_mask() {
	// analyse image
	for (int y = 0; y < logo.getHeight(); y++) {
		for (int x = 0; x < logo.getWidth(); x++) {
			ofColor pixelColor = logo.getColor(x, y);
			// Wenn der Pixel "sichtbar" ist, blockieren wir ihn
			//logoMask[y * logo.getWidth() + x] = (pixelColor.a > 0); // Alpha > 0 => sichtbarer Pixel
			if (logo.getColor(x, y).a > 0) { // Wenn Pixel nicht transparent ist
				logoPositions.push_back(ofVec2f(x + logo_left, y + logo_top));
				cout << "transparent ";
			}
		}
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
