#include "intro_scene.h"

#include <algorithm>
#include <execution>
#include <vector>

#include <ofVec2f.h>

IntroScene::IntroScene() {
    flow_field.resize(cols * rows); // initialize vector field
    z_offset = 0.0;

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

    line_shader.load("shaders/line_shader.vert", "shaders/line_shader.frag", "shaders/line_shader.geom");
    line_shader.setGeometryInputType(GL_LINES);
    line_shader.setGeometryOutputType(GL_LINE_STRIP);
    line_shader.setGeometryOutputCount(2);

    logo_svg.load("resources/media_error_logo_lines.svg");
    logo_in_outs_svg.load("resources/logo_in_and_out_lines.svg");
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

    pixel_shader.load("shaders/pixel_intro");

    // compute shader
    for (int i = 0; i < particles.size(); ++i) {
        simple_particles.emplace_back(particles[i].position);
        particle_tails.emplace_back(particles[i].position.x, particles[i].position.y);
    }

    // Load and compile compute shader
    compute_shader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/compute_shader.comp");
    compute_shader.linkProgram();

    // Allocate and upload pixel data to GPU
    particle_buffer.allocate(simple_particles, GL_DYNAMIC_DRAW);
    particle_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);

    flow_field_buffer.allocate(flow_field, GL_DYNAMIC_DRAW);
    flow_field_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

    logo_vectors_buffer.allocate(all_logo_vectors, GL_DYNAMIC_DRAW);
    logo_vectors_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);
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


    // Bind the buffer and dispatch the compute shader
    particle_buffer.updateData(0, simple_particles);
    flow_field_buffer.updateData(0, flow_field);
    particle_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    flow_field_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    logo_vectors_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);

    compute_shader.begin();
    compute_shader.setUniform1i("fieldWidth", rows);
    compute_shader.setUniform1i("fieldHeight", cols);
    compute_shader.setUniform1f("fieldScale", resolution);

    compute_shader.setUniform2f("logo_center", logo_center);
    compute_shader.setUniform1f("logo_radius", logo_radius);

    compute_shader.setUniform1i("number_of_particles", simple_particles.size());
    compute_shader.setUniform1i("logo_vectors_size", all_logo_vectors.size());

    compute_shader.dispatchCompute(simple_particles.size(), 1, 1);
    compute_shader.end();

    // Map the buffer to access the updated pixel data
    SimpleParticle *mappedPixels = particle_buffer.map<SimpleParticle>(GL_READ_ONLY);
    if (mappedPixels) {
        memcpy(simple_particles.data(), mappedPixels, simple_particles.size() * sizeof(SimpleParticle));
        particle_buffer.unmap();
    }

    for (std::size_t i = 0; i < simple_particles.size(); ++i) {
        simple_particles[i].position += simple_particles[i].velocity;
        particle_tails[i].update_position(simple_particles[i].position);
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


        // visualized flowing field
        /* for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                ofVec2f vec = flow_field[y * cols + x];
                ofPushMatrix();
                ofTranslate(x * resolution, y * resolution);
                ofDrawLine(0, 0, vec.x * resolution * 0.5, vec.y * resolution * 0.5);
                ofPopMatrix();
            }
        }*/

        // draw particles

        // pixel_shader.begin();
        // pixel_shader.setUniform1f("pixelationAmount", 50.0f);
        // pixel_shader.setUniform1f("quality", 0.5f);


        for (auto &particle: particles) {
            // particle.draw();
        }

        // pixel_shader.end();

        // for (const auto &pixel: simple_particles) {
        //     ofDrawCircle(pixel.position, 2);
        // }

        line_shader.begin();
        line_shader.setUniform1f("uMaxLength", 10);
        line_shader.setUniformTexture("logo_texture", logo_fbo.getTexture(), 0);

        for (auto &particle: particle_tails) {
            particle.draw();
        }
        line_shader.end();

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
