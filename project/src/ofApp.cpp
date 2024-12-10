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
	//create_logo_vectors();
	create_circle_vectors();

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
			float angle = ofNoise(x * 0.1, y * 0.1, zOffset) * TWO_PI; // Noise erzeugt Winkel
			flowField[y * cols + x] = ofVec2f(cos(angle), sin(angle));
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

		// Partikel in der Nähe der Logo-Vektoren anziehen und entlang der Vektoren bewegen
		for (auto& logoVec : logo_vectors) {
			float distance = p.position.distance(logoVec.first);  // Abstand zum Logo-Vektor
			if (distance < attractionRadius) {
				// Vektor entlang der Logo-Richtung anwenden
				ofVec2f attractionForce = logoVec.second * 0.2; // Stärke der Anziehung
				p.applyForce(attractionForce);

				// Bewegung entlang der Logo-Richtung (nicht an einem Punkt "kleben bleiben")
				// Die Partikel sollten entlang des Vektors weiterziehen
				// Berechnung der Richtung des Vektors zum aktuellen Partikel
				ofVec2f directionToVector = logoVec.first - p.position;
				directionToVector.normalize();

				// Optionale zusätzliche Bewegungskraft entlang der Richtung des Vektors
				p.applyForce(directionToVector * 0.1);
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
	ofSetColor(0, 255, 0); // green lines für das Logo
	for (auto& logo_vec : logo_vectors) {
	//	ofDrawLine(logo_vec.first, logo_vec.first + logo_vec.second * 10);
	}

	// Circle-Vektoren zeichnen
	ofSetColor(0, 255, 0); // green lines für das Logo
	for (auto& circle_vec : circle_vectors) {
		ofDrawLine(circle_vec.first, circle_vec.first + circle_vec.second * 10);
	}
}

void ofApp::create_logo_vectors() {
	for (int y = 0; y < logo.getHeight(); y++) {
		for (int x = 0; x < logo.getWidth(); x++) {

			// Helligkeit des aktuellen Pixels
			int currentBrightness = logo.getColor(x, y).getBrightness();

			// Helligkeit der Nachbarpixel
			int rightBrightness = logo.getColor(x + 1, y).getBrightness();
			int downBrightness = logo.getColor(x, y + 1).getBrightness();

			// Wenn ein signifikanter Unterschied in der Helligkeit besteht, ist das eine Kante
			if (abs(currentBrightness - rightBrightness) > 20 || abs(currentBrightness - downBrightness) > 20) {
				// Richtung berechnen (Differenz zwischen Nachbarpixeln)
				int dx = rightBrightness - currentBrightness;
				int dy = downBrightness - currentBrightness;

				ofVec2f direction(dx, dy);
				if (direction.length() > 0) {
					direction.normalize();
					logo_vectors.push_back(make_pair(ofVec2f(x + logo_left, y + logo_top), direction));
				}
			}
		}
	}
}

void ofApp::create_circle_vectors() {
	ofVec2f center = ofVec2f(screen_width / 2, screen_height / 2); // Setze den Mittelpunkt in die Bildschirmmitte
	int numVectors = 100;
	float radius = 150;

	// Erstelle die Vektoren entlang des Kreisumfangs
	for (int i = 0; i < numVectors; i++) {
		float angle = ofMap(i, 0, numVectors, 0, TWO_PI); // Verteile die Winkel gleichmäßig von 0 bis 2?
		float x = center.x + cos(angle) * radius;
		float y = center.y + sin(angle) * radius;

		ofVec2f position(x, y);

		// Beispiel: Vektor zeigt nach außen vom Mittelpunkt
		ofVec2f direction = position - center;
		direction.normalize(); // Normiere den Vektor auf Länge 1

		circle_vectors.push_back(std::make_pair(position, direction));
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
