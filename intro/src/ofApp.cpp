#include "ofApp.h"
#include <ofVec2f.cpp>

#include <algorithm>
#include <execution>

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    flow_field.resize(cols * rows); // initialize vector field
    z_offset = 0.0;

    logo_svg.load(image);
    logo_left = screen_width / 2 - logo_svg.getWidth() / 2;
    logo_right = screen_width / 2 + logo_svg.getWidth() / 2;
    logo_top = screen_height / 2 - logo_svg.getHeight() / 2;
    logo_bottom = screen_height / 2 + logo_svg.getHeight() / 2;

    create_logo_vectors();

    for (int i = 0; i < num_particles; i++) {
        particles.push_back(Particle(ofRandom(ofGetWidth()), ofRandom(ofGetHeight())));
    }
}

//--------------------------------------------------------------
void ofApp::update() {
    z_offset += 0.01; // animation offset gets increased

    // vectors get calculated by Perlin noise
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            float posX = x * resolution;
            float posY = y * resolution;

            // Prüfen, ob der aktuelle Punkt innerhalb des Logo-Bereichs liegt
            if (posX >= logo_position.x - logo_width / 2 && posX <= logo_position.x + logo_width / 2 &&
                posY >= logo_position.y - logo_height / 2 && posY <= logo_position.y + logo_height / 2) {
                // Überspringen, wenn innerhalb des Logos
                continue;
            }

            float angle = ofNoise(x * 0.1, y * 0.1, z_offset) * TWO_PI; // noise creates angle
            flow_field[y * cols + x] = ofVec2f(cos(angle), sin(angle));
        }
    }

    // calculates particle movement in parallel threads
    std::for_each(std::execution::par_unseq, particles.begin(), particles.end(), [&](Particle &particle) {
        particle.apply_repulsion(particles, repulsion_radius, repulsion_strength);

        auto x_index = static_cast<int>(particle.position.x / resolution);
        auto y_index = static_cast<int>(particle.position.y / resolution);
        auto index = y_index * cols + x_index;

        // using perlin noise as force
        if (index >= 0 && index < flow_field.size()) {
            ofVec2f force = flow_field[index];
            particle.apply_force(force);
        }

        // force, that pulls particles to the logo
        for (auto &logo_vec: logo_vectors) {
            float distance = particle.position.distance(logo_vec.first); // distance to logo_vector position
            // particles within distance, get attracted by logo_vector
            if (distance < attraction_radius) {
                ofVec2f attraction_force =
                        logo_vec.second * 0.2; // power of attractionforce calculated by logo_vector direction
                particle.apply_force(attraction_force);

                // movement along the logo (not sticking on one point)
                // calculation direction from vector to current particle
                ofVec2f direction_to_vector = logo_vec.first - particle.position;
                direction_to_vector.normalize();
                particle.apply_force(direction_to_vector * 0.1);
            }
        }
    });

    // updating particles
    for (auto &particle: particles) {
        particle.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    ofScale(0.5, 0.5);
    ofTranslate(-logo_svg.getHeight() / 2, -logo_svg.getHeight() / 2);
    // logo_svg.draw();
    ofPopMatrix();


    //// visualized flowing field
    // for (int y = 0; y < rows; y++) {
    //	for (int x = 0; x < cols; x++) {
    //		ofVec2f vec = flow_field[y * cols + x];
    //		ofPushMatrix();
    //		ofTranslate(x * resolution, y * resolution);
    //		ofDrawLine(0, 0, vec.x * resolution * 0.5, vec.y * resolution * 0.5);
    //		ofPopMatrix();
    //	}
    // }

    // draw particles
    for (auto &particle: particles) {
        particle.draw();
    }

    // drawing logo_vectors
    ofSetColor(0, 255, 0); // green
    for (auto &logo_vec: logo_vectors) {
        // ofDrawLine(logo_vec.first, logo_vec.first + logo_vec.second * 10);
    }
}

void ofApp::create_logo_vectors() {

    // SVG-Bounding-Box berechnen
    ofRectangle bounding_box;

    for (int i = 0; i < logo_svg.getNumPath(); i++) {
        ofPath path = logo_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        vector<ofPolyline> outlines = path.getOutline();

        for (auto &outline: outlines) {
            outline = outline.getResampledBySpacing(1); // Resample für gleichmäßige Punkte
            for (auto &point: outline) {
                point *= logo_scale;
            }
            bounding_box.growToInclude(outline.getBoundingBox());
        }
    }

    // Berechne den Offset, um das Logo in die Mitte des Screens zu verschieben
    ofVec2f offset((ofGetWidth() - bounding_box.getWidth()) / 2 - bounding_box.getLeft(),
                   (ofGetHeight() - bounding_box.getHeight()) / 2 - bounding_box.getTop());

    // Logo-Vektoren generieren und den Offset anwenden
    for (int i = 0; i < logo_svg.getNumPath(); i++) {
        ofPath path = logo_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        vector<ofPolyline> outlines = path.getOutline();

        for (auto &outline: outlines) {
            outline = outline.getResampledBySpacing(1); // Optional: Punkte gleichmäßig verteilen
            for (int j = 0; j < outline.size() - 1; j++) {
                ofVec2f start = ofVec2f(outline[j]) * logo_scale + offset;
                ofVec2f end = ofVec2f(outline[j + 1]) * logo_scale + offset;

                // Richtung berechnen entlang des Pfades
                ofVec2f direction = end - start;
                direction.normalize();

                // Anpassung der Richtung: Richtung von rechts nach links erzwingen
                if (direction.x > 0) {
                    direction *= -1; // Drehe den Vektor um
                }

                logo_vectors.push_back(std::make_pair(start, direction));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
