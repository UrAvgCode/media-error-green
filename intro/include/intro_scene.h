#pragma once

#include <string>
#include <utility>
#include <vector>

#include <ofMain.h>
#include <ofxSvg.h>

#include <scene.h>
#include "particle.h"
#include "simple_particle.h"
#include "particle_tail.h"

class IntroScene : public Scene {
  public:
    IntroScene();

    void update() override;
    void render() override;

  private:
    const int screen_width = ofGetWidth();
    const int screen_height = ofGetHeight();

    // Flow Field
    const int resolution = 20; // size of every cell
    const int cols = ofGetWidth() / resolution;
    const int rows = ofGetHeight() / resolution;

    std::vector<glm::vec2> flow_field;
    float z_offset; // for animated Perlin noise

    // particles
    std::vector<Particle> particles;
    const int num_particles = 2000;
    const float repulsion_radius = 25;
    const float repulsion_strength = 5;

    const float attraction_radius = 30;
    const float attraction_strength = 1000;

    float logo_tolerance = 10.0f;

    // logo
    ofxSVG logo_svg;
    ofxSVG logo_in_outs_svg;
    const float logo_scale = 1.0;
    string logo_image = "logo_lines4.svg";
    string logo_in_outs_image = "logo_in_and_outs.svg";
    std::vector<pair<glm::vec2, glm::vec2>> logo_vectors;
    std::vector<pair<glm::vec2, glm::vec2>> logo_in_outs_vectors;
    std::vector<std::pair<glm::vec2, glm::vec2>> all_logo_vectors;

    ofxSVG logo_picture;
    ofFbo logo_fbo;

    ofShader line_shader;

    ofVec2f logo_position;
    float logo_width;
    float logo_height;
    int logo_left, logo_right, logo_top, logo_bottom;
    ofVec2f logo_center;
    float logo_radius;
    int logo_margin = 30;

    ofShader pixel_shader;

    vector<SimpleParticle> simple_particles;
    vector<ParticleTail> particle_tails;
    ofShader compute_shader;
    ofBufferObject particle_buffer;
    ofBufferObject flow_field_buffer;
    ofBufferObject logo_vectors_buffer;

    void create_logo_vectors();
    void create_logo_in_outs_vectors();
};
