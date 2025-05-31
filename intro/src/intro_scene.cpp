#include "intro_scene.h"

#include <algorithm>
#include <execution>
#include <vector>

#include <ofVec2f.h>

IntroScene::IntroScene() {
    flow_field_resolution = 20;
    flow_field_cols = ofGetWidth() / flow_field_resolution;
    flow_field_rows = ofGetHeight() / flow_field_resolution;
    flow_field.resize(flow_field_cols * flow_field_rows);

    flow_field_offset = 0;

    logo_picture.load("resources/media_error_logo.svg");
    logo_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);

    logo_fbo.begin();
    {
        ofPushMatrix();
        ofTranslate(0.5f * ofGetWidth(), 0.5f * ofGetHeight());
        ofTranslate(-logo_picture.getHeight() / 2.0f, -logo_picture.getHeight() / 2.0f);
        logo_picture.draw();
        ofPopMatrix();
    }
    logo_fbo.end();

    particle_trail_shader.load("shaders/particle_trail_shader.vert", "shaders/particle_trail_shader.frag",
                               "shaders/particle_trail_shader.geom");
    particle_trail_shader.setGeometryInputType(GL_LINES);
    particle_trail_shader.setGeometryOutputType(GL_LINE_STRIP);
    particle_trail_shader.setGeometryOutputCount(2);

    particle_pixel_shader.load("shaders/particle_pixel_shader");

    particle_draw_fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);

    logo_svg.load("resources/media_error_logo_lines.svg");
    logo_in_outs_svg.load("resources/logo_in_and_out_lines.svg");
    logo_position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);

    float logo_left = ofGetWidth() / 2.0f - logo_svg.getWidth() / 2.0f;
    float logo_right = ofGetWidth() / 2.0f + logo_svg.getWidth() / 2.0f;
    float logo_top = ofGetHeight() / 2.0f - logo_svg.getHeight() / 2.0f;
    float logo_bottom = ofGetHeight() / 2.0f + logo_svg.getHeight() / 2.0f;

    create_logo_vectors();
    create_logo_in_outs_vectors();
    // ´combine both vectorlists
    all_logo_vectors = logo_vectors;
    all_logo_vectors.insert(all_logo_vectors.end(), logo_in_outs_vectors.begin(), logo_in_outs_vectors.end());

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
    logo_margin = 30;
}

//--------------------------------------------------------------
void IntroScene::update() {
    flow_field_offset += 0.01; // animation offset gets increased

    // vectors get calculated by Perlin noise
    for (int y = 0; y < flow_field_rows; y++) {
        for (int x = 0; x < flow_field_cols; x++) {

            float pos_x = x * flow_field_resolution;
            float pos_y = y * flow_field_resolution;

            // Prüfen, ob der aktuelle Punkt innerhalb des Logo-Kreises liegt
            float distance_to_logo_center = ofVec2f(pos_x, pos_y).distance(logo_center);
            if (distance_to_logo_center <= logo_radius + logo_margin) {
                continue;
            }

            float angle = ofNoise(x * 0.1, y * 0.1, flow_field_offset) * TWO_PI; // noise creates angle
            flow_field[y * flow_field_cols + x] = ofVec2f(cos(angle), sin(angle));
        }
    }

    // calculates particle movement in parallel threads
    std::for_each(std::execution::par_unseq, particles.begin(), particles.end(), [&](Particle &particle) {
        float distance_to_logo = particle.position.distance(logo_center);
        if (distance_to_logo <= logo_radius) {
            particle.apply_repulsion(particles, repulsion_radius, repulsion_strength);
        }

        auto x_index = static_cast<int>(particle.position.x / flow_field_resolution);
        auto y_index = static_cast<int>(particle.position.y / flow_field_resolution);
        auto index = y_index * flow_field_cols + x_index;

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
    particle_draw_fbo.begin();
    {
        ofClear(0);

        particle_trail_shader.begin();
        {
            particle_trail_shader.setUniform1f("uMaxLength", 10);
            particle_trail_shader.setUniformTexture("logo_texture", logo_fbo.getTexture(), 0);
            for (auto &particle: particles) {
                particle.draw();
            }
        }
        particle_trail_shader.end();
    }
    particle_draw_fbo.end();

    frame_buffer.begin();
    {
        particle_pixel_shader.begin();
        particle_pixel_shader.setUniform1f("block_size", 5);
        particle_pixel_shader.setUniform1f("quality", 0.5f);

        particle_pixel_shader.setUniform1f("random", ofRandom(1));
        particle_pixel_shader.setUniform1f("random_width", ofRandomWidth());
        particle_pixel_shader.setUniform1f("random_height", ofRandomHeight());

        line_position = (line_position + 2) % ofGetHeight();
        particle_pixel_shader.setUniform1i("line_position", line_position);

        particle_pixel_shader.setUniform2f("texture_size", ofGetWidth(), ofGetHeight());

        particle_draw_fbo.draw(0, 0);

        particle_pixel_shader.end();
    }
    frame_buffer.end();
}

void IntroScene::reset_particles() {
    for (auto &particle: particles) {
        particle.reset_position();
    }
}

void IntroScene::create_logo_vectors() {

    // SVG-Bounding-Box berechnen
    ofRectangle bounding_box;

    for (int i = 0; i < logo_svg.getNumPath(); i++) {
        ofPath path = logo_svg.getPathAt(i);
        path.setPolyWindingMode(OF_POLY_WINDING_ODD);

        std::vector<ofPolyline> outlines = path.getOutline();

        for (auto &outline: outlines) {
            outline = outline.getResampledBySpacing(4); // Resample für gleichmäßige Punkte
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
            outline = outline.getResampledBySpacing(4); // Optional: Punkte gleichmäßig verteilen
            for (int j = 0; j < outline.size() - 1; j++) {
                ofVec2f start = ofVec2f(outline[j]) + offset;
                ofVec2f end = ofVec2f(outline[j + 1]) + offset;

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
            outline = outline.getResampledBySpacing(4); // Resample für gleichmäßige Punkte
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
            outline = outline.getResampledBySpacing(4); // Optional: Punkte gleichmäßig verteilen
            for (int j = 0; j < outline.size() - 1; j++) {
                ofVec2f start = ofVec2f(outline[j]) + offset;
                ofVec2f end = ofVec2f(outline[j + 1]) + offset;

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
