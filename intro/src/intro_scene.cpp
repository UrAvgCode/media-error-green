#include "intro_scene.h"

#include <algorithm>
#include <execution>
#include <vector>

#include <ofVec2f.h>

IntroScene::IntroScene() {
    flow_field.resize(flow_field_cols * flow_field_rows); // initialize vector field
    flow_field_offset = 0.0;

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

    render_particles_shader.load("shaders/render_particles");

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

    for (std::size_t i = 0; i < num_particles; ++i) {
        auto velocity = glm::vec2{ofRandom(-2, 2), ofRandom(-2, 2)};
        auto position = glm::vec2{ofRandomWidth(), ofRandomHeight()};
        auto positions = std::array<glm::vec2, 100>({position});

        particles.push_back({velocity, positions});
    }

    // Load and compile compute shader
    compute_shader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/calculate_particle_velocity.comp");
    compute_shader.linkProgram();

    move_particles_shader.setupShaderFromFile(GL_COMPUTE_SHADER, "shaders/calculate_particle_position.comp");
    move_particles_shader.linkProgram();

    // Allocate and upload pixel data to GPU
    particle_buffer.allocate(particles, GL_DYNAMIC_COPY);
    particle_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    particle_buffer.bind(GL_ARRAY_BUFFER);

    flow_field_buffer.allocate(flow_field, GL_DYNAMIC_DRAW);
    flow_field_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

    logo_vectors_buffer.allocate(all_logo_vectors, GL_DYNAMIC_DRAW);
    logo_vectors_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);

    // Create VBO and VAO for rendering
    glGenVertexArrays(1, &particle_vao);
    glBindVertexArray(particle_vao);

    // Enable and describe position attribute (assuming layout(location = 0) in vertex shader)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *) sizeof(glm::vec2));

    glBindVertexArray(0);
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

    // Bind the buffer and dispatch the compute shader
    flow_field_buffer.updateData(0, flow_field);

    particle_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    flow_field_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    logo_vectors_buffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);

    compute_shader.begin();
    compute_shader.setUniform1i("fieldWidth", flow_field_rows);
    compute_shader.setUniform1i("fieldHeight", flow_field_cols);
    compute_shader.setUniform1f("fieldScale", flow_field_resolution);

    compute_shader.setUniform2f("logo_center", logo_center);
    compute_shader.setUniform1f("logo_radius", logo_radius);

    compute_shader.setUniform1i("number_of_particles", particles.size());
    compute_shader.setUniform1i("logo_vectors_size", all_logo_vectors.size());

    compute_shader.dispatchCompute((particles.size() + 1023) / 1024, 1, 1);
    compute_shader.end();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    move_particles_shader.begin();
    move_particles_shader.dispatchCompute((particles.size() + 1023) / 1024, 1, 1);
    move_particles_shader.end();
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void IntroScene::render() {
    particle_draw_fbo.begin();
    {
        ofClear(0);
        glClear(GL_COLOR_BUFFER_BIT);

        render_particles_shader.begin();
        glBindVertexArray(particle_vao);
        glDrawArrays(GL_POINTS, 0, particles.size());
        glBindVertexArray(0);
        render_particles_shader.end();
    }
    particle_draw_fbo.end();

    frame_buffer.begin();
    {
        particle_pixel_shader.begin();

        particle_pixel_shader.setUniform1f("block_size", 5);
        particle_pixel_shader.setUniform1f("quality", 0.5f);
        particle_draw_fbo.draw(0, 0);

        particle_pixel_shader.end();
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
            outline = outline.getResampledBySpacing(8); // Resample für gleichmäßige Punkte
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
            outline = outline.getResampledBySpacing(8); // Optional: Punkte gleichmäßig verteilen
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
            outline = outline.getResampledBySpacing(8); // Resample für gleichmäßige Punkte
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
            outline = outline.getResampledBySpacing(8); // Optional: Punkte gleichmäßig verteilen
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
