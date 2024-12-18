#include "ofApp.h"
#include <ofVec2f.cpp>


//--------------------------------------------------------------
void ofApp::setup(){
	cols = ofGetWidth() / resolution;
	rows = ofGetHeight() / resolution;
	flowField.resize(cols * rows); // initialize vector field
	zOffset = 0.0;

	logo_svg.load(image);
	logo_left = screen_width / 2 - logo_svg.getWidth() / 2;
	logo_right = screen_width / 2 + logo_svg.getWidth() / 2;
	logo_top = screen_height / 2 - logo_svg.getHeight() / 2;
	logo_bottom = screen_height / 2 + logo_svg.getHeight() / 2;

	create_logo_vectors();
	create_circle_vectors();

	for (int i = 0; i < num_particles; i++) {
		particles.push_back(Particle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight())));
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	zOffset += 0.01; // animation offset gets increased

	ofVec2f logoPosition = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2); // Logo zentriert
	float logoWidth = logo_svg.getWidth() * logo_scale;  // Passe die Breite des Logos an
	float logoHeight = logo_svg.getHeight() * logo_scale; // Passe die Höhe des Logos an

	// vectors get calculated by Perlin noise
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			
			float posX = x * resolution;
			float posY = y * resolution;

			// Prüfen, ob der aktuelle Punkt innerhalb des Logo-Bereichs liegt
			if (posX >= logoPosition.x - logoWidth / 2 && posX <= logoPosition.x + logoWidth / 2 &&
				posY >= logoPosition.y - logoHeight / 2 && posY <= logoPosition.y + logoHeight / 2) {
				// Überspringen, wenn innerhalb des Logos
				continue;
			}

			float angle = ofNoise(x * 0.1, y * 0.1, zOffset) * TWO_PI; // noise creates angle
			flowField[y * cols + x] = ofVec2f(cos(angle), sin(angle));
		}
	}

	// updating particles
	for (auto& particle : particles) {

		// Repulsionskraft anwenden
		particle.apply_repulsion(particles, repulsion_radius, repulsion_strength); // Radius und Stärke anpassen

		int xIndex = floor(particle.position.x / resolution);
		int yIndex = floor(particle.position.y / resolution);
		int index = yIndex * cols + xIndex;

		// using perlin noise as force
		if (index >= 0 && index < flowField.size()) {
			ofVec2f force = flowField[index];
			particle.apply_force(force);
		}

		// calculate logo force
		float attractionRadius = 20;
		float attractionStrength = 1000;

		// force, that pulls particles to the circle
		//for (auto& circle_vec : circle_vectors) {
		//	float distance = particle.position.distance(circle_vec.first);  // distance to circle_vector position
		//	// particles within distance, get attracted by circle_vector
		//	if (distance < attractionRadius) {
		//		ofVec2f attractionForce = circle_vec.second * 0.2; // power of attractionforce calculated by circle_vector direction
		//		particle.apply_force(attractionForce);

		//		// movement along the circle (not sticking on one point)
		//		// calculation direction from vector to current particle
		//		ofVec2f directionToVector = circle_vec.first - particle.position;
		//		directionToVector.normalize();
		//		particle.apply_force(directionToVector * 0.1);
		//	}
		//}

		// force, that pulls particles to the logo
		for (auto& logo_vec : logo_vectors) {
			float distance = particle.position.distance(logo_vec.first);  // distance to logo_vector position
			// particles within distance, get attracted by logo_vector
			if (distance < attractionRadius) {
				ofVec2f attractionForce = logo_vec.second * 0.2; // power of attractionforce calculated by logo_vector direction
				particle.apply_force(attractionForce);

				// movement along the logo (not sticking on one point)
				// calculation direction from vector to current particle
				ofVec2f directionToVector = logo_vec.first - particle.position;
				directionToVector.normalize();
				particle.apply_force(directionToVector * 0.1);
			}
		}

		particle.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
	ofScale(0.5, 0.5);
	ofTranslate(-logo_svg.getHeight()/2, -logo_svg.getHeight()/2);
	//logo_svg.draw();
	ofPopMatrix();


	//// visualized flowing field
	//for (int y = 0; y < rows; y++) {
	//	for (int x = 0; x < cols; x++) {
	//		ofVec2f vec = flowField[y * cols + x];
	//		ofPushMatrix();
	//		ofTranslate(x * resolution, y * resolution);
	//		ofDrawLine(0, 0, vec.x * resolution * 0.5, vec.y * resolution * 0.5);
	//		ofPopMatrix();
	//	}
	//}

	// draw particles
	for (auto& particle : particles) {
		particle.draw();
	}

	// drawing logo_vectors
	ofSetColor(0, 255, 0); // green
	for (auto& logo_vec : logo_vectors) {
		//ofDrawLine(logo_vec.first, logo_vec.first + logo_vec.second * 10);
	}

	// drawing circle_vectors
	ofSetColor(0, 255, 0); // green
	for (auto& circle_vec : circle_vectors) {
		//ofDrawLine(circle_vec.first, circle_vec.first + circle_vec.second * 10);
	}
}

void ofApp::create_logo_vectors() {

	// SVG-Bounding-Box berechnen
	ofRectangle boundingBox;

	for (int i = 0; i < logo_svg.getNumPath(); i++) {
		ofPath path = logo_svg.getPathAt(i);
		path.setPolyWindingMode(OF_POLY_WINDING_ODD);

		vector<ofPolyline> outlines = path.getOutline();

		for (auto& outline : outlines) {
			outline = outline.getResampledBySpacing(1); // Resample für gleichmäßige Punkte
			for (auto& point : outline) {
				point *= logo_scale;
			}
			boundingBox.growToInclude(outline.getBoundingBox());
		}
	}

	// Berechne den Offset, um das Logo in die Mitte des Screens zu verschieben
	ofVec2f offset((ofGetWidth() - boundingBox.getWidth()) / 2 - boundingBox.getLeft(),
		(ofGetHeight() - boundingBox.getHeight()) / 2 - boundingBox.getTop());

	// Logo-Vektoren generieren und den Offset anwenden
	for (int i = 0; i < logo_svg.getNumPath(); i++) {
		ofPath path = logo_svg.getPathAt(i);
		path.setPolyWindingMode(OF_POLY_WINDING_ODD);

		vector<ofPolyline> outlines = path.getOutline();

		for (auto& outline : outlines) {
			outline = outline.getResampledBySpacing(1); // Optional: Punkte gleichmäßig verteilen
			for (int j = 0; j < outline.size() - 1; j++) {
				ofVec2f start = ofVec2f(outline[j]) * logo_scale + offset;
				ofVec2f end = ofVec2f(outline[j + 1]) * logo_scale + offset;

				// Richtung berechnen entlang des Pfades
				ofVec2f direction = end - start;
				direction.normalize();

				// Anpassung der Richtung: Richtung von rechts nach links erzwingen
				if (direction.x > 0 ) {
					direction *= -1; // Drehe den Vektor um
				}

				logo_vectors.push_back(std::make_pair(start, direction));
			}
		}
	}
}

void ofApp::create_circle_vectors() {
	ofVec2f center = ofVec2f(screen_width / 2, screen_height / 2); // middlepoint of circle in the middle of screen
	int numVectors = 100;
	float radius = 150;

	// creates vectors around the circle
	for (int i = 0; i < numVectors; i++) {
		float angle = ofMap(i, 0, numVectors, 0, TWO_PI); // place vectors evenly on circle. two_pi because its a circle
		float x = center.x + cos(angle) * radius;
		float y = center.y + sin(angle) * radius;

		ofVec2f position(x, y);

		ofVec2f direction(-sin(angle), cos(angle)); // direction of vectors follows circle
		direction.normalize(); // norm vector to length 1

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
