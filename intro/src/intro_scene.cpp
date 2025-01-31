#include "intro_scene.h"

#include <algorithm>
#include <execution>
#include <vector>

#include <ofVec2f.h>

IntroScene::IntroScene() {
    flow_field.resize(cols * rows); // initialize vector field
    z_offset = 0.0;

    logo_svg.load(logo_image);
    logo_in_outs_svg.load(logo_in_outs_image);
    logo_position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);
    logo_width = logo_svg.getWidth() * logo_scale;
    logo_height = logo_svg.getHeight() * logo_scale;

    logo_left = screen_width / 2 - logo_svg.getWidth() / 2;
    logo_right = screen_width / 2 + logo_svg.getWidth() / 2;
    logo_top = screen_height / 2 - logo_svg.getHeight() / 2;
    logo_bottom = screen_height / 2 + logo_svg.getHeight() / 2;

    create_logo_vectors();
    create_logo_in_outs_vectors();
    // ´combine both vectorlists
    all_logo_vectors = logo_vectors;
    all_logo_vectors.insert(all_logo_vectors.end(), logo_in_outs_vectors.begin(), logo_in_outs_vectors.end());

    for (int i = 0; i < num_particles; i++) {
        particles.emplace_back(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()));
    }

    // Berechne die Bounding Box des Logos
    ofRectangle boundingBoxLogo;
    for (int i = 0; i < logo_svg.getNumPath(); i++) {
        ofPath path = logo_svg.getPathAt(i);
        vector<ofPolyline> outlines = path.getOutline();
        for (auto &outline: outlines) {
            boundingBoxLogo.growToInclude(outline.getBoundingBox());
        }
    }

    // Mittelpunkt und Radius des Kreises berechnen
    logo_center = ofVec2f(boundingBoxLogo.getCenter().x + logo_left, boundingBoxLogo.getCenter().y + logo_top);
    logo_radius = (std::max(boundingBoxLogo.getWidth(), boundingBoxLogo.getHeight()) / 2.0f);
    Particle::logo_center = logo_center;
    Particle::logo_radius = logo_radius;
}

//--------------------------------------------------------------
void IntroScene::update() {
    z_offset += 0.01; // animation offset gets increased

    // vectors get calculated by Perlin noise
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            float pos_x = x * resolution;
            float pos_y = y * resolution;

            //// Prüfen, ob der aktuelle Punkt innerhalb des Logo-Bereichs liegt
            // if (pos_x >= logo_position.x - logo_width / 2 && pos_x <= logo_position.x + logo_width / 2 &&
            //     pos_y >= logo_position.y - logo_height / 2 && pos_y <= logo_position.y + logo_height / 2) {
            //     // Überspringen, wenn innerhalb des Logos
            //     continue;
            // }

            // Prüfen, ob der aktuelle Punkt innerhalb des Logo-Kreises liegt
            float distance_to_logo_center = ofVec2f(pos_x, pos_y).distance(logo_center);
            if (distance_to_logo_center <= logo_radius + logo_margin) {
                // Überspringen, wenn innerhalb des Logos
                continue;
            }

            float angle = ofNoise(x * 0.1, y * 0.1, z_offset) * TWO_PI; // noise creates angle
            flow_field[y * cols + x] = ofVec2f(cos(angle), sin(angle));
        }
    }

    // calculates particle movement in parallel threads
    std::for_each(std::execution::par_unseq, particles.begin(), particles.end(), [&](Particle &particle) {
        float distance_to_logo = particle.position.distance(logo_center);
        if (distance_to_logo <= logo_radius) {
            particle.apply_repulsion(particles, repulsion_radius, repulsion_strength);
        }

        auto x_index = static_cast<int>(particle.position.x / resolution);
        auto y_index = static_cast<int>(particle.position.y / resolution);
        auto index = y_index * cols + x_index;

        // using perlin noise as force
        if (index >= 0 && index < flow_field.size()) {
            ofVec2f force = flow_field[index];
            particle.apply_force(force);
        }

        // force, that pulls particles to the logo
        for (auto &logo_vec: all_logo_vectors) {
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
        particle.update(logo_vectors, logo_tolerance);
    }
}

void IntroScene::render() {
    frame_buffer.begin();
    {
        ofClear(0);

        // ofPushMatrix();
        // ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        // ofScale(0.5, 0.5);
        // ofTranslate(-logo_svg.getHeight() / 2, -logo_svg.getHeight() / 2);
        //  logo_svg.draw();
        // ofPopMatrix();


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
        // ofSetColor(0, 255, 0); // green
        /*for (auto &logo_vec: all_logo_vectors) {
             ofDrawLine(logo_vec.first, logo_vec.first + logo_vec.second * 10);
        }*/
    }
    frame_buffer.end();
}

void IntroScene::create_logo_vectors() {

    // SVG-Bounding-Box berechnen
    ofRectangle bounding_box;

    for (int i = 0; i < logo_svg.getNumPath(); i++) {
        ofPath path = logo_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        std::vector<ofPolyline> outlines = path.getOutline();

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

        std::vector<ofPolyline> outlines = path.getOutline();

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

                logo_vectors.emplace_back(start, direction);
            }
        }
    }
}

void IntroScene::create_logo_in_outs_vectors() {

    // SVG-Bounding-Box berechnen
    ofRectangle bounding_box;

    for (int i = 0; i < logo_in_outs_svg.getNumPath(); i++) {
        ofPath path = logo_in_outs_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        std::vector<ofPolyline> outlines = path.getOutline();

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
    for (int i = 0; i < logo_in_outs_svg.getNumPath(); i++) {
        ofPath path = logo_in_outs_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        std::vector<ofPolyline> outlines = path.getOutline();

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

                logo_in_outs_vectors.emplace_back(start, direction);
            }
        }
    }
}
